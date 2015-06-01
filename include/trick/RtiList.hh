/*
    PURPOSE:
        (Real Time Injector)
*/

#ifndef RTILIST_HH
#define RTILIST_HH

#include <vector>
#include "trick/RtiEvent.hh"

namespace Trick {

class RtiList {
    public:
        void execute( bool debug ) ;

        void print_list() ;

        std::vector<RtiEventBase * > event_list ;
} ;

}

#endif


