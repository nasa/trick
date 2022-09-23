/*
    PURPOSE:
        (Trick runtime simulation executive scheduler.)
*/

#ifndef SCHEDULER_HH
#define SCHEDULER_HH

#include <string>
#include <map>
#include <stdio.h>

#include "trick/ScheduledJobQueue.hh"
#include "trick/SimObject.hh"

namespace Trick {

    /**
     * This class is the base scheduler class.  All schedulers in sim_services inherit from this class.
     * This provides declarations of base functionality required for sim_services schedulers.
     *
     * @author Robert W. Bailey
     * @author Many other Trick developers of the past who did not add their names.
     * @author Alexander S. Lin
     *
     * @date Jan. 2010
     *
     */

    class Scheduler {

        protected:

            /** Maps a string name, i.e. "initialization" to an integer class id.\n */
            std::map<std::string, int > class_map ;                     /**< trick_io(**) */

            /** Maps an integer class id to corresponding ScheduledJobQueue.\n */
            std::map<int, Trick::ScheduledJobQueue * > class_to_queue ;   /**< trick_io(**) */

        public:

            virtual ~Scheduler() ;

            /**
             * Default implementation called when sim_objects are added to the
             * simulation.  Adds sim_object jobs to the scheduler queues.
             * @param in_object - incoming sim_object
             * @return always 0
             */
            virtual int add_sim_object( Trick::SimObject * in_object ) ;

            /**
             * Default implementation called when sim_objects are removed from
             * the simulation.  Removes simobject jobs from the scheduler queues.
             * @param in_object - incoming sim_object
             * @return always 0
             */
            virtual int delete_sim_object( Trick::SimObject * in_object ) ;

            /**
             * Formats cyclic jobs to be written to the S_job_execution file
             * @param fp - open file pointer to write jobs data
             * @param curr_queue - queue to parse and write to file
             * @return always 0
             */
            int write_sched_queue( FILE * fp , Trick::ScheduledJobQueue * curr_queue ) ;

            /**
             * Formats non cyclic jobs to be written to the S_job_execution file
             * @param fp - open file pointer to write jobs data
             * @param curr_queue - queue to parse and write to file
             * @return always 0
             */
            int write_non_sched_queue( FILE * fp , Trick::ScheduledJobQueue * curr_queue ) ;

            /**
             * Writes the S_job_execution file which details the jobs known to the scheduler at the end of initialization.
             * @param fp - either open file pointer to write jobs data, or NULL
             * @return always 0
             */
            virtual int write_s_job_execution(FILE * fp) = 0 ;

            /**
             * Adds an instrumentation job with the name in_job before target_job (if specified),
             * otherwise adds in_job before each job in the initialization and scheduled queues.
             * Requirement [@ref r_exec_instrument_0]
             * @param instrument_job - the instrument job
             * @return always 0
             */
            virtual int instrument_job_before(Trick::JobData * instrument_job ) = 0 ;

            /**
             * Adds an instrumentation job with the name in_job after target_job (if specified),
             * otherwise adds in_job after each job in the initialization and scheduled queues.
             * Requirement [@ref r_exec_instrument_2]
             * @param instrument_job - the instrument job
             * @return always 0
             */
            virtual int instrument_job_after(Trick::JobData * instrument_job ) = 0 ;

            /**
             * Removes an instrumentation job with the name in_job in the initialization and scheduled queues.
             * Requirement [@ref r_exec_instrument_3]
             * @param in_job - name of the instrument job
             * @return always 0
             */
            virtual int instrument_job_remove(std::string in_job ) = 0 ;

    } ;

}

#endif

