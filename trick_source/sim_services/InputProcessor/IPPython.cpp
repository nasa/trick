/*
   PURPOSE: ( Python input processor )
   REFERENCE: ( Trick Simulation Environment )
   ASSUMPTIONS AND LIMITATIONS: ( None )
   CLASS: ( N/A )
   LIBRARY DEPENDENCY: ( None )
   PROGRAMMERS: ( Alex Lin NASA 2009 )
*/

#include <Python.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "trick/IPPython.hh"
#include "trick/MemoryManager.hh"
#include "trick/exec_proto.hh"
#include "trick/exec_proto.h"

Trick::IPPython * the_pip ;

//Constructor
Trick::IPPython::IPPython() : Trick::InputProcessor::InputProcessor() , units_conversion_msgs(true) {
    the_pip = this ;
    return ;
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
    for ( aim_it = trick_MM->alloc_info_map_begin() ; aim_it != trick_MM->alloc_info_map_end() ; aim_it++ ) {
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
            PyRun_SimpleString(ss.str().c_str()) ;
        }
    }
}

bool Trick::IPPython::get_units_conversion_msgs() {
    return units_conversion_msgs ;
}

void Trick::IPPython::shoot_the_units_conversion_messenger(bool onoff) {
    units_conversion_msgs = !onoff ;
}

//Initialize and run the Python input processor on the user input file.
int Trick::IPPython::init() {
    /** @par Detailed Design: */

    FILE *input_fp ;
    std::string shortcut ;
    std::string verify_command ;
    int ret ;
    std::string error_message ;
    pthread_mutexattr_t m_attr ;

    /* Initialize a mutex to protect python processing for var server and events. */
    pthread_mutexattr_init(&m_attr) ;
    pthread_mutexattr_settype(&m_attr, PTHREAD_MUTEX_RECURSIVE) ;
    pthread_mutex_init(&ip_mutex , &m_attr) ;

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

    /* Import simulation specific routines into interpreter. */
    PyRun_SimpleString(
     "import sys\n"
     "import os\n"
     "import struct\n"
     "import binascii\n"
     "sys.path.append(os.getcwd())\n"
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
        ret = PyRun_SimpleString("sys.settrace(trick.traceit)") ;
    }

    if ( (ret = PyRun_SimpleFile(input_fp, input_file.c_str())) !=  0 ) {
        exec_terminate_with_return(ret , __FILE__ , __LINE__ , "Input Processor error\n" ) ;
    }

    if ( verify_input ) {
       exec_terminate_with_return(ret , __FILE__ , __LINE__ , "Input file verification complete\n" ) ;
    }

    fclose(input_fp) ;
    return(0) ;

}

//Command to parse the given string.
int Trick::IPPython::parse(std::string in_string) {

    int ret ;
    pthread_mutex_lock(&ip_mutex);
    in_string += "\n" ;
    ret = PyRun_SimpleString(in_string.c_str()) ;
    pthread_mutex_unlock(&ip_mutex);

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

    pthread_mutex_lock(&ip_mutex);
    in_string =  std::string("trick_ip.ip.return_val = ") + in_string + "\n" ;
    // Running the simple string will set return_val.
    PyRun_SimpleString(in_string.c_str()) ;
    cond_return_val = return_val ;
    pthread_mutex_unlock(&ip_mutex);

    return 0 ;

}

//Restart job that reloads event_list from checkpointable structures
int Trick::IPPython::restart() {
    /* Make shortcut names for all known sim_objects. */
    get_TMM_named_variables() ;
    return 0 ;
}

int Trick::IPPython::shutdown() {
    if ( Py_IsInitialized() ) {
        Py_Finalize();
    }
    return(0) ;
}

void shoot_the_units_conversion_messenger() {
    the_pip->shoot_the_units_conversion_messenger(true) ;
}

void revive_the_units_conversion_messenger() {
    the_pip->shoot_the_units_conversion_messenger(false) ;
}

int check_units_conversion_messenger_for_signs_of_life() {
    return the_pip->get_units_conversion_msgs() ;
}
