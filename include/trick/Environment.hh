/*
    PURPOSE:
        (Trick compiletime environment variables)
*/

#ifndef ENVIRONMENT_HH
#define ENVIRONMENT_HH

#include <string>
#include <map>

#include <stdio.h>

namespace Trick {

    /**
     * This class saves the Trick environment variables at compile-time.  The variables
     * are saved in a routine found in S_source.cpp.
     *
     * @author Alexander S. Lin
     *
     * @date June 2010
     *
     */

    class Environment {

        protected:

            /** Local map of trick environment variables and their values.\n */
            std::map<std::string, std::string > local_env ;          /**< trick_io(**) */

        public:

            Environment() ;

            /**
             Adds an environment variable to the map.
             @param variable_name - environment varaible name
             @param value - contents of the environment variable
             @return always 0
             */
            int add_var(std::string variable_name, std::string value) ;

            /**
             Gets an environment variable from the map.
             @param variable_name - environment varaible name
             @return char * string of the variable if it exists, otherwise NULL.
             */
            const char * get_var(std::string variable_name) ;

            /**
             Prints the map of environment varaibles to the incoming file pointer
             @param fp - open file pointer
             @return always 0
             */
            int print_vars( FILE *fp ) ;

    } ;

}

#endif

