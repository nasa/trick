/*
    PURPOSE:
        (Real Time Injector)
*/

#ifndef _RTILIST_HH
#define _RTILIST_HH

#include <vector>
#include "RtiEvent.hh"

namespace Trick {

class RtiList {
    public:
        void execute( bool debug ) ;

        void print_list() ;

        std::vector<RtiEventBase * > event_list ;
} ;

}

#endif


