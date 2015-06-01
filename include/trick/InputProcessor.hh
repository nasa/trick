/*
    PURPOSE:
        (Trick runtime simulation executive parameter definition.)
*/

#ifndef INPUTPROCESSOR_HH
#define INPUTPROCESSOR_HH

#include <string>

namespace Trick {

    class InputProcessor {

        public:

            int verify_input ;                    /* -- verify input */

            std::string input_file ;              /* -- Simulation input data file name */

            InputProcessor() ;

            virtual ~InputProcessor() {} ;

            // Initialization jobs
            int process_sim_args() ;

            // parse a string usually from variable server
            virtual int parse(std::string in_string) = 0 ;

            // shutdown jobs
            virtual int shutdown() ;

    } ;

}

#endif

