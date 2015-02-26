/*
    PURPOSE:
        (Trick runtime simulation executive parameter definition.)
*/

/*
 * $Id: InputProcessor.hh 3537 2014-05-27 22:49:43Z alin $
 */

#ifndef _INPUTPROCESSOR_HH_
#define _INPUTPROCESSOR_HH_

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

