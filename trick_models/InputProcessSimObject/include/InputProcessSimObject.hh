/*
PURPOSE:
    ( InputProcessor "Sim object" )
PROGRAMMERS:
    (((Alex Lin) (NASA) (2009)))
LIBRARY_DEPENDENCIES:
    ((../src/InputProcessSimObject.cpp))
*/

/*
 * $Id: IntegSimObject.hh 416 2009-11-18 22:32:12Z lin $
 */

#ifndef _INPUTPROCESSSIMOBJECT_HH_
#define _INPUTPROCESSSIMOBJECT_HH_

#include <vector>
#include <string>
#include <map>

#include "sim_services/SimObject/include/SimObject.hh"

namespace Trick {

    class InputProcessSimObject : public SimObject {

        public:

            std::string python_call_function ;
            int return_value ;

            InputProcessSimObject( std::string ) ;

            /* call_function hand coded for jobs to be called by scheduler */
            virtual int call_function( Trick::JobData * curr_job ) ;
            virtual double call_function_double( Trick::JobData * curr_job ) ;

    } ;

}

#endif

