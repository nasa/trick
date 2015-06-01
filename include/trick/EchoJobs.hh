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

#ifndef ECHOJOBS_HH
#define ECHOJOBS_HH

#include "trick/JobData.hh"

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

