/*******************************************************************************
*                                                                              *
* Trick Simulation Environment Software                                        *
*                                                                              *
* Copyright (c) 1996,1997 LinCom Corporation, Houston, TX                      *
* All rights reserved.                                                         *
*                                                                              *
* Copyrighted by LinCom Corporation and proprietary to it. Any unauthorized    *
* use of Trick Software including source code, object code or executables is   *
* strictly prohibited and LinCom assumes no liability for such actions or      *
* results thereof.                                                             *
*                                                                              *
* Trick Software has been developed under NASA Government Contracts and        *
* access to it may be granted for Government work by the following contact:    *
*                                                                              *
* Contact: Charles Gott, Branch Chief                                          *
*          Simulation and Graphics Branch                                      *
*          Automation, Robotics, & Simulation Division                         *
*          NASA, Johnson Space Center, Houston, TX                             *
*                                                                              *
*******************************************************************************/
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

/* 
 * $Id: Executive.hh 416 2009-11-18 22:32:12Z lin $
 */

#ifndef _DEBUGPAUSE_HH_
#define _DEBUGPAUSE_HH_

#include <string>
#include <semaphore.h>

#include "sim_services/SimObject/include/JobData.hh"

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

