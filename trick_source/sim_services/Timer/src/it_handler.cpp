/*
   PURPOSE: ( Interval timer/external interrupt alarm signal handler )

   PROGRAMMERS: (((Robert W. Bailey) (LinCom) (April 1992) (--) (Realtime)))
*/

#include "sim_services/Timer/include/ITimer.hh"

/**
@relates Trick::ITimer
@brief Signal handler for itimer based sleep timer
@param sig - signal number
@details
-# call sem_post() on the itimer semaphore
*/
void it_handler(int sig __attribute__ ((unused))) {
    // Tell the scheduler to wait up from its sleepy time.
    get_itimer()->semaphore_post() ;
}
