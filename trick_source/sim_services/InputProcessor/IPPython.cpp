/*
   PURPOSE: ( Python input processor )
   REFERENCE: ( Trick Simulation Environment )
   ASSUMPTIONS AND LIMITATIONS: ( None )
   CLASS: ( N/A )
   LIBRARY DEPENDENCY: ( None )
   PROGRAMMERS: ( Alex Lin NASA 2009 )
*/

#include "trick/IPPython.hh"

#include "trick/InputProcessor.hh"
#include "trick/MemoryManager.hh"
#include "trick/exec_proto.h"
#include "trick/io_alloc.h"
#include "trick/message_proto.h"
#include "trick/message_type.h"

#include <Python.h>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <pthread.h>
#include <sstream>
#include <string>
#include <unistd.h>

Trick::IPPython * the_pip ;

//Constructor
Trick::IPPython::IPPython() : Trick::InputProcessor::InputProcessor() , units_conversion_msgs(true) {
    the_pip = this ;
    return ;
}


// Need to save the state of the main thread to allow child threads to run PyRun variants.
static PyThreadState *_save = NULL;

// Number of threads currently inside parse()/parse_condition(), i.e. threads that hold
// the Python GIL or are waiting to acquire it. See GILGuard and shutdown() below.
static std::atomic<int> active_parse_count(0);

namespace
{
    /**
     * Holds the Python GIL for a scope, with three guarantees that the bare
     * PyGILState_Ensure()/PyGILState_Release() pair does not provide.
     *
     * 1. Thread cancellation is disabled for as long as the GIL is held. Without this a
     *    pthread_cancel() (ThreadBase::cancel_thread(), used during shutdown) can destroy the
     *    thread between Ensure and Release, leaving the GIL owned by a thread that no longer
     *    exists. Nobody can ever reacquire it after that. A cancel requested while
     *    cancellation is disabled simply stays pending and is delivered once the GIL has been
     *    released, which is exactly the behaviour we want.
     *
     * 2. The GIL is released even if an exception unwinds out of the scope. Trick's
     *    exec_terminate() throws Trick::ExecutiveException from inside PyRun_SimpleString
     *    (VariableServerSessionThread_loop.cpp catches it), and the previous straight-line
     *    code skipped PyGILState_Release() on that path, leaking the GIL.
     *
     * 3. active_parse_count tracks how many threads are in the interpreter, so that
     *    IPPython::shutdown() can tell whether reacquiring the GIL is safe or would block
     *    forever.
     */
    class GILGuard
    {
        public:
            GILGuard()
            {
                pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_cancel_state);
                // Incremented before Ensure() so that a thread blocked *waiting* for the GIL
                // is counted too. Erring towards "someone is in the interpreter" is the safe
                // direction: the worst case is that we skip Py_Finalize() unnecessarily.
                ++active_parse_count;
                gstate = PyGILState_Ensure();
            }

            ~GILGuard()
            {
                PyGILState_Release(gstate);
                --active_parse_count;
                pthread_setcancelstate(old_cancel_state, NULL);
            }

            GILGuard(const GILGuard&)            = delete;
            GILGuard& operator=(const GILGuard&) = delete;

        private:
            PyGILState_STATE gstate;
            int old_cancel_state;
    };

}

/**
@details
-# Loops through all of the memorymanager allocations testing if a name handle was given.
 -# If a name and a user type_name were given to the allocation
  -# If the user_type_name is not a Trick core class, prefixed with "Trick::"
  -# Create a python statement to assign the python name to an address: <name> = trick.castAsTYPE(int(<address>))
  -# Run the statement in the python interpreter
*/
void Trick::IPPython::get_TMM_named_variables() {
    //std::cout << "top level names at initialization" << std::endl ;
    Trick::ALLOC_INFO_MAP_ITER aim_it ;
    for ( aim_it = trick_MM->alloc_info_map_begin() ; aim_it != trick_MM->alloc_info_map_end() ; ++aim_it ) {
        ALLOC_INFO * alloc_info = (*aim_it).second ;
        if ( alloc_info->name != NULL and alloc_info->user_type_name != NULL ) {
            std::stringstream ss ;
            std::string user_type_name = alloc_info->user_type_name ;
            size_t start_colon ;
            while ( ( start_colon = user_type_name.find("::") ) != std::string::npos ) {
                user_type_name.replace( start_colon , 2 , "__" ) ;
            }
            // The castAs method may not exist if the class was hidden from SWIG (#ifndef SWIG).
            // Use a try/except block to test if the method exists or not.  If it doesn't exist
            // don't worry about it.  Also only assign python variable if it is pointing to
            // something python doesn't owns.  Otherwise we could free the object we're trying to assign.
            ss << "try:" << std::endl ;
            ss << "    if '" << alloc_info->name << "' not in globals() or " ;
            ss << alloc_info->name << ".thisown == False:" <<  std::endl ;
            ss << "        " << alloc_info->name << " = " ;
            ss << "trick.castAs" << user_type_name << "(int(" << alloc_info->start << "))" << std::endl ;
            ss << "except AttributeError:" << std::endl ;
            ss << "    pass" << std::endl ;
            GILGuard gil_guard;
            PyRun_SimpleString(ss.str().c_str());
        }
    }
}

bool Trick::IPPython::get_units_conversion_msgs() {
    return units_conversion_msgs ;
}

void Trick::IPPython::shoot_the_units_conversion_messenger(bool onoff) {
    if ( onoff ) {
        message_publish(MSG_WARNING, "Units conversion messages can no longer be suppressed.\n") ;
    }
}

//Initialize and run the Python input processor on the user input file.
int Trick::IPPython::init() {
    /** @par Detailed Design: */

    FILE *input_fp ;
    int ret ;
    std::string error_message ;

    // Run Py_Initialze first for python 2.x
#if PY_VERSION_HEX < 0x03000000
    Py_Initialize();
#endif

    /* Run the Swig generated routine in S_source_wrap.cpp. */
    init_swig_modules() ;

    // Run Py_Initialze after init_swig_modules for python 3.x
#if PY_VERSION_HEX >= 0x03000000
    Py_Initialize();
#endif

    // The following PyRun_ calls do not require the PyGILState guards because no threads are launched
    /* Import simulation specific routines into interpreter. */
    PyRun_SimpleString(
     "import sys\n"
     "import os\n"
     "import struct\n"
     "import binascii\n"
     "if 'VIRTUAL_ENV' in os.environ:\n"
     "    sys.path.append(os.path.join(os.environ['VIRTUAL_ENV'], \"lib\", f\"python{sys.version_info.major}.{sys.version_info.minor}\", \"site-packages\"))\n"
     "sys.path.append(os.getcwd() + '/trick.zip')\n"
     "sys.path.append(os.path.join(os.environ['TRICK_HOME'], 'share/trick/pymods'))\n"
     "sys.path += map(str.strip, os.environ['TRICK_PYTHON_PATH'].split(':'))\n"
     "import trick\n"
     "sys.path.append(os.getcwd() + \"/Modified_data\")\n"
    ) ;

    /* Make shortcut names for all known sim_objects. */
    get_TMM_named_variables() ;

    /* An input file is not required, if the name is empty just return. */
    if ( input_file.empty() ) {
        return(0) ;
    }

    if ((input_fp = fopen(input_file.c_str(), "r")) == NULL) {
        error_message = "No input file found named " + input_file ;
        exec_terminate_with_return(-1 , __FILE__ , __LINE__ , error_message.c_str() ) ;
    }

    /* Read and parse the input file. */
    if ( verify_input ) {
        PyRun_SimpleString("sys.settrace(trick.traceit)") ;
    }

    /* Read and parse the input file. */
    if ( save_input ) {
        PyRun_SimpleString("trick.open_input_file_log()") ;
        PyRun_SimpleString("sys.settrace(trick.traceittofile)") ;
    }

    if ( (ret = PyRun_SimpleFile(input_fp, input_file.c_str())) !=  0 ) {
        exec_terminate_with_return(ret , __FILE__ , __LINE__ , "Input Processor error\n" ) ;
    }

    if ( verify_input ) {
       std::stringstream ss ;
       ss << "import hashlib" << std::endl ;
       ss << "input_file = " << "'" << input_file.c_str() << "'" << std::endl;
       ss << "print('{0} SHA1: {1}'.format(input_file,hashlib.sha1(open(input_file, 'rb').read()).hexdigest()))" << std::endl ;
       PyRun_SimpleString(ss.str().c_str()) ;
       exec_terminate_with_return(ret , __FILE__ , __LINE__ , "Input file verification complete\n" ) ;
    }

    if ( save_input ) {
        PyRun_SimpleString("trick.close_input_file_log()") ;
        PyRun_SimpleString("sys.settrace(None)") ;
    }

    fclose(input_fp) ;

    // Release the GIL from the main thread.
    Py_UNBLOCK_THREADS

    return(0) ;
}

//Command to parse the given string.
int Trick::IPPython::parse(std::string in_string) {

    int ret ;
    in_string += "\n" ;

    GILGuard gil_guard;
    ret = PyRun_SimpleString(in_string.c_str());

    return ret ;

}

/**
 @details
 The incoming statement is assumed to be a conditional fragment, i.e. "a > b".  We need
 to get the return value of this fragment by setting the return value of it to a known
 variable name in the input processor.  We can then assign that return value to the
 incoming return_value reference.

-# Lock the input processor mutex
-# Create a complete statement that assigns the conditional fragment to our return value
-# parse the condition
-# copy the return value to the incoming cond_return_value
-# Unlock the input processor mutex
*/
int Trick::IPPython::parse_condition(std::string in_string, int & cond_return_val ) {

    in_string =  std::string("trick_ip.ip.return_val = ") + in_string + "\n" ;
    // Running the simple string will set return_val.
    int py_ret;
    {
        GILGuard gil_guard;
        py_ret = PyRun_SimpleString(in_string.c_str());
    }
    cond_return_val = return_val ;

    return py_ret ;

}

//Restart job that reloads event_list from checkpointable structures
int Trick::IPPython::restart() {
    /* Make shortcut names for all known sim_objects. */
    get_TMM_named_variables() ;
    return 0 ;
}

int Trick::IPPython::shutdown() {

    if ( Py_IsInitialized() ) {
        // If any thread is still inside parse()/parse_condition() then it owns the GIL,
        // or is queued for it, and may never give it back -- the usual cause is a
        // variable server command that blocks inside a model call, since SWIG does not
        // release the GIL around wrapped calls. Reacquiring the GIL below would then
        // block forever.
        //
        // Skipping Py_Finalize() in that case is safe. The process is about to exit, so
        // the OS reclaims the interpreter's memory regardless; all we give up is Python's
        // orderly teardown (atexit handlers, __del__ methods). That is a far better
        // outcome than wedging the sim, and it lets Executive::shutdown() carry on and
        // report the real return code instead of the run appearing to hang.
        int in_interpreter = active_parse_count;
        if (in_interpreter > 0)
        {
            message_publish(
                MSG_WARNING,
                "Trick::IPPython::shutdown() skipping Py_Finalize(): %d thread(s) still executing in the Python "
                "interpreter and holding the GIL. The Python interpreter will not be finalized.\n",
                in_interpreter);
            return (0);
        }

        // Obtain the GIL so that we can shut down properly
        // Check if the thread state is actually saved before trying to restore it
        // Python thread state is NULL when using JIT Input
        if (_save != NULL)
        {
            Py_BLOCK_THREADS _save = NULL;
        }
        Py_Finalize();
    }
    return(0) ;
}

//TODO: remove units conversion messenger routines in 2021
void shoot_the_units_conversion_messenger() {
    message_publish(MSG_WARNING, "shoot_the_units_conversion_messenger() is deprecated\n") ;
    the_pip->shoot_the_units_conversion_messenger(true) ;
}

void revive_the_units_conversion_messenger() {
    message_publish(MSG_WARNING, "revive_the_units_conversion_messenger() is deprecated\n") ;
    the_pip->shoot_the_units_conversion_messenger(false) ;
}

int check_units_conversion_messenger_for_signs_of_life() {
    message_publish(MSG_WARNING, "check_units_conversion_messenger_for_signs_of_life() is deprecated\n") ;
    return the_pip->get_units_conversion_msgs() ;
}
//END TODO:
