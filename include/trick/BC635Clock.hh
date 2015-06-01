/*
PURPOSE:
    ( BC635 CTE Clock )
PROGRAMMERS:
     ((Alex Lin) (NASA) (April 2009) (--) (port to c++)))
*/

#ifndef BC635CLOCK_HH
#define BC635CLOCK_HH

#include <string>
#include "trick/Clock.hh"

#ifdef _BC635
#ifndef TRICK_ICG
#ifndef SWIG
extern "C" {
#include "bcuser.h"
}
#endif
#endif
#endif

namespace Trick {

    class BC635Clock : public Clock {

        public:

            BC635Clock() ;
            ~BC635Clock() ;

            /* getters and setters */
            void set_mode(unsigned int in_mode) ;
            unsigned int get_mode() ;

            /** @copybrief Trick::Clock::clock_init() */
            virtual int clock_init() ;

            /** @copybrief Trick::Clock::wall_clock_time() */
            virtual long long wall_clock_time() ;

            /** @copybrief Trick::Clock::clock_spin() */
            virtual long long clock_spin(long long req_time) ;

            /** @copybrief Trick::Clock::clock_stop() */
            virtual int clock_stop() ;

        private:
#ifdef _BC635
#ifndef TRICK_ICG
#ifndef SWIG
            BC_PCI_HANDLE h_bc635 ;
#endif
#endif
#endif
            unsigned int mode ;

    } ;

}

#endif
