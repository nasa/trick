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

#ifndef _ECHOJOBS_HH_
#define _ECHOJOBS_HH_

#include "sim_services/SimObject/include/JobData.hh"

namespace Trick {

    class EchoJobs {

        private:
            /** Set to true to echo names of called jobs.\n */
            bool echo_job_flag ;                  /**< trick_units(--) */

        public:
            /**
             @brief This is the constructor of the EchoJobs class.
             */
            EchoJobs() ;

            /**
             @brief Instrumentation class job that prints out the simulation time and job name for every job when echo_jobs is on.
             @param curr_job - pointer to current instrument job that points to the job to echo
             @return always 0
             */
            int echo_job(Trick::JobData * curr_job) ;

            /**
             @brief @userdesc Command to set echo_job_flag to true which turns on the echo_jobs printout.
             Calls Trick::Executive::instrument_job_before to insert the echo_jobs routine before every job to be echoed.
             @par Python Usage:
             @code trick.echo_jobs_on() @endcode
             @return always 0
             */
            int echojobs_on() ;

            /**
             @brief @userdesc Command to set echo_job_flag to false which turns off the echo_jobs printout.
             Calls Trick::Executive::instrument_job_remove to remove the echo_job routine from all job queues that it was inserted in.
             @par Python Usage:
             @code trick.echo_jobs_off() @endcode
             @return always 0
             */
            int echojobs_off() ;

    } ;

}

#endif

