/*
    PURPOSE:
        (Trick runtime simulation executive parameter definition.)
    REFERENCE:
        (((Bailey, R.W, and Paddock, E.J.) (Trick Simulation Environment)
          (NASA:JSC #37943)
          (JSC / Engineering Directorate / Automation and Robotics Division)
          (June 1994) (--)))
    ASSUMPTIONS AND LIMITATIONS:
        ((Only 64 levels of nested input data file inclusion.))
    PROGRAMMERS:
        (((Robert W. Bailey) (LinCom) (4/92) (--) (Realtime))
         ((Robert W. Bailey) (LinCom) (6/1/91) (Trick-CR-00000) (Initial Release)))
*/

#ifndef DEBUGPAUSE_HH
#define DEBUGPAUSE_HH

#include <string>
#include <semaphore.h>

#include "trick/JobData.hh"

namespace Trick {

    class DebugPause {

        public:
            /** Set to true to pause before every job call during sim execution.\n */
            bool debug_pause_flag ;               /**< trick_units(--) */

            /**
             @brief This is the constructor of the DebugPause class.
             */
            DebugPause() ;

            /**
             @brief Instrumentation class job that pauses before each job when debug_pause is on.
             Waits on a semapahore before execution continues.
             @param curr_job - pointer to current instrument job that points to the job to pause at
             @return always 0
             */
            int debug_pause(Trick::JobData * curr_job) ;

            /**
             @brief Command to post the semaphore so that execution continues after pause.
             @return always 0
             */
            int debug_signal() ;

            /**
             @brief Command to set debug_pause_flag to true which turns on the debug pausing.
             Calls Trick::Executive::instrument_job_before to insert the debug_pause routine before every job to pause at.
             @return always 0
             */
            int debug_pause_on() ;

            /**
             @brief Command to set debug_pause_flag to false which turns off the debug pausing.
             Calls Trick::Executive::instrument_job_remove to remove the debug_pause routine from all job queues that it was inserted in.
             @return always 0
             */
            int debug_pause_off() ;

        private:
            /** Semaphore used to control pausing.\n */
            sem_t * debug_sem ;                   /**< trick_io(**) trick_units(--) */
            /** Semaphore name for debug_sem.\n */
            std::string sem_name ;                /**< trick_io(**) trick_units(--) */

    } ;

}

#endif

