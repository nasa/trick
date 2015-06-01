/*
PURPOSE:
    ( TPRO CTE Clock )
PROGRAMMERS:
     ((Alex Lin) (NASA) (April 2009) (--) (port to c++)))
*/


#ifndef TPROCTECLOCK_HH
#define TPROCTECLOCK_HH

#include <string>
#include "trick/Clock.hh"

#ifdef _TPRO_CTE
extern "C" {
#include "tpro.h"
}
#endif

namespace Trick {

    class TPROCTEClock : public Clock {

        public:

            TPROCTEClock() ;
            ~TPROCTEClock() ;

            /** @copybrief Trick::Clock::clock_init() */
            virtual int clock_init() ;

            /** @copybrief Trick::Clock::wall_clock_time() */
            virtual long long wall_clock_time() ;

            /** @copybrief Trick::Clock::clock_spin() */
            virtual long long clock_spin(long long req_time) ;

            /** @copybrief Trick::Clock::clock_stop() */
            virtual int clock_stop() ;

            std::string dev_name ;

        private:
#ifdef _TPRO_CTE
            TPRO_BoardObj *pBoard ;
#endif

    } ;

}

#endif
