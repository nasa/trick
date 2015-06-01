/*
PURPOSE:
    ( gettimeofday Clock )
PROGRAMMERS:
    (((Robert W. Bailey) (LinCom) (April 1992) (--) (Realtime.))
     ((Robert W. Bailey) (LinCom) (7/22/92) (--) (--))
     ((Eddie J. Paddock) (MDSSC) (Oct 1992) (--) (Realtime.))
     ((Alex Lin) (NASA) (April 2009) (--) (port to c++)))

*/

#ifndef ITIMER_HH
#define ITIMER_HH

#include <time.h>
#include <sys/time.h>
#include <semaphore.h>
#include <string>

#include "trick/Timer.hh"

namespace Trick {

    class ITimer : public Timer {

        public:

            ITimer() ;
            ~ITimer() ;

            /** Post the semaphore value from the interrupt handler */
            void semaphore_post() ;

            /** @copybrief Trick::Timer::init() */
            virtual int init() ;

            /** @copybrief Trick::Timer::start() */
            virtual int start(double frame_time) ;

            /** @copybrief Trick::Timer::reset() */
            virtual int reset(double frame_time) ;

            /** @copybrief Trick::Timer::stop() */
            virtual int stop() ;

            /** @copybrief Trick::Timer::pause() */
            virtual int pause() ;

            /** @copybrief Trick::Timer::shutdown() */
            virtual int shutdown() ;

        protected:

            struct timespec res;       /* ** resolution of the clock */
            struct timespec timeout;   /* ** timeout value of some kind */
            sem_t * semaphore;         /* ** Posted by SIGALRM handler to tell sched loop to wake up. */
            std::string sem_name ;     /* ** name of the semaphore */

    } ;

}

Trick::ITimer * get_itimer() ;
void it_handler(int sig) ;

#endif
