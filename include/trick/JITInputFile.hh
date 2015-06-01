/**
PURPOSE:
    (blah.)
LIBRARY DEPENDENCY:
    ((../src/JITInputFile.cpp))
*/

#ifndef JITINPUTFILE_HH
#define JITINPUTFILE_HH

#include <dlfcn.h>
#include <string>
#include <queue>
#include <map>

#include "trick/JITEvent.hh"
#include "trick/JobData.hh"

namespace Trick {

/**
 * This class is used in a map in the JITInputFile class to hold
 * a library name and a handle to the library contents.
 *
 * @author Alexander S. Lin
 *
 * @date April 2014
 *
 */
struct JITLibInfo {
    JITLibInfo() : handle(NULL) {} ;

    /** name of library */
    std::string library_name ;

    /** handle returned by dlopen to the library */
    void * handle ;

    void * find_symbol(std::string sym) { return dlsym(handle, sym.c_str()) ; }
} ;

/**
 * This class compiles/loads/runs functions out of dynamically loaded shared libraries.
 * C++ source code input files may be used in place of a python input file.
 * Source code is compiled and linked into a shared library.  The library
 * is dynamically loaded and functions out of the library may be run
 * Libraries may be pre-compiled and loaded directly.
 *
 * Calls from this class may be called from python input files.
 *
 * @author Alexander S. Lin
 *
 * @date April 2014
 *
 */

class JITInputFile {

    public:
        JITInputFile() ;

        int process_sim_args() ;

        /**
         Called as an intialization job to compile and run the input file
         @return 0 for success, compilation error code if compilation fails.
        */
        int init() ;

        /**
         Compile the incoming file name
         @param file_name - the name of the C++ file.
         @return 0 for success, compilation error code if compilation fails.
        */
        int compile(std::string file_name) ;

        /**
         Run a function out of a loaded library
         @param library_name - the name of the library
         @param run_function - the name of function to run
         @return 0 for success, error code if library not loaded or function not found.
        */
        int run(std::string library_name , std::string run_function ) ;

        /**
         Convienience function to call compile and run
         @param file_name - the name of the C++ file.
         @param run_function - the name of function to run
         @return 0 for success, error code from either compile or run function if an error occurred.
        */
        int compile_and_run(std::string file_name, std::string run_function = "run_me" ) ;

        /**
         Adds a precompiled library to the list of known libraries (does not attempt to open library)
         @param lib_name - the name of the shared library
         @return 0 for success
        */
        int add_library(std::string lib_name) ;

        void * find_symbol(std::string sym) ;
    private:
        /** C++ input file from the command line if one was specified. */
        std::string input_file ;

        /** Map of source file name to structue holding library name and library handle */
        std::map< std::string , JITLibInfo > file_to_libinfo_map ;

} ;

}
#endif
