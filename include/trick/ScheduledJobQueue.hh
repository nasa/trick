/*
    PURPOSE:
        (Class to hold queue of scheduled jobs)
*/

#ifndef SCHEDULEDJOBQUEUE_HH
#define SCHEDULEDJOBQUEUE_HH

#include <string>

#include "trick/JobData.hh"

namespace Trick {

    /**
     * The ScheduledJobQueue is specifically for the scheduled jobs.  It does not
     * allocate memory during normal cycling through jobs and is considerably
     * faster than the generalized priority_queue.
     *
     * @author Robert W. Bailey
     * @author many other Trick developers of the past who did not add their names.
     * @author Alexander S. Lin
     */

    class ScheduledJobQueue {

            friend class ScheduledJobQueueTest ;

        public:

            /**
             * @brief This is the constructor of the class.  It initializes
             * the most member data to 0, or TRICK_MAX_LONG_LONG for next_job_time
             */
            ScheduledJobQueue() ;

            /**
             * @brief This is the destructor of the class.  It frees list.
             */
            ~ScheduledJobQueue() ;

            /**
             * @brief Adds a new job into list. The exact placement of job is dependent on
             * job_class, phase , sim_object order , and finally job order in sim object.
             * @param in_job - Job to add to the list
             * @return always 0.
             */
            int push(JobData * in_job ) ;

            /**
             * @brief Adds a new job into list ignoring the sim_object id.  This is useful for
             * schedulers that redefine the order sim_objects are processed and are only
             * concerned with phase ordering.
             * @param in_job - Job to add to the list
             * @return always 0.
             */
            int push_ignore_sim_object(JobData * in_job ) ;

            /**
             * @brief Removes a job from the list if present.
             * @param in_job - Job to remove to the list
             * @return always 0.
             */
            int remove(JobData * in_job ) ;

            /**
             * @brief Gets the curr_index.
             * @return curr_index.
             */
            unsigned int get_curr_index() ;

            /**
             * @brief Sets the curr_index to value.
             * @param ii - Value of index.
             * @return always 0.
             */
            int set_curr_index( unsigned int ii ) ;

            /**
             * @brief Resets the curr_index data member to 0.
             * @return always 0.
             */
            int reset_curr_index() ;

            /**
             * @brief Returns the size of the queue
             * @return the number of elements in the queue.
             */
            unsigned int size() ;

            /**
             * @brief Tests if the list is empty or not.
             * @return 0 if the list is empty or 1 if the list has at least one job.
             */
            bool empty() ;

            /**
             * @brief Clears the list of jobs.
             * @return 0
             */
            int clear() ;

            /**
             * @brief Clears the list of jobs.
             * @return 0
             */
            int execute_all_jobs() ;

            /**
             * @brief Write all job information out as scheduled jobs in a format suitable for S_job_execution
             * @return 0
             */
            int write_sched_queue( FILE * fp ) ;

            /**
             * @brief Write all job information out as non scheduled jobs in a format suitable for S_job_execution
             * @return 0
             */
            int write_non_sched_queue( FILE * fp ) ;

            /**
             * @brief Adds the incoming instrumentation job before target job if specified, or all jobs in list.
             * Will reallocate list to accommodate additional instrumentation jobs.
             * @param instrumentation_job  - name of the instrument job
             * @return number of insertions made
             */
            int instrument_before(JobData * instrumentation_job) ;

            /**
             * @brief Adds the incoming instrumentation job after target job if specified, or all jobs in the list.
             * Will reallocate list to accommodate additional instrumentation jobs.
             * @param instrumentation_job - name of the instrument job
             * @return number of insertions made
             */
            int instrument_after(JobData * instrumentation_job) ;

            /**
             * @brief Removes all jobs in the list that match the name job_name.
             * If in_event is specified, only remove this event's instrument job.
             * @param job_name - name of the instrument job
             * @return always 0
             */
            int instrument_remove(std::string job_name) ;

            /**
             * @brief Gets the job pointed to by current_index, the top of the list.
             * @return top job in the list, or NULL if the list is empty.
             */
            JobData * top() ;

            /**
             * @brief Gets the job pointed to by current_index, the top of the list.
             * Increments current_index to advance to next job.
             * @return top job in the list, or NULL if the list is empty or the end of list reached.
             */
            JobData * get_next_job() ;

            /**
             * @brief Finds the next job where the next job call time equals the incoming time.
             * @param time - the requested time next job call time
             * @return next job in list who's next call time matches incoming time, or NULL if no match found.
             */
            JobData * find_next_job(long long time ) ;

            /**
             * @brief Finds the next job where the next job call time equals the incoming time.  Does not
             * update any information so find_next_job can still find the job returned by this function.
             * @param time - the requested time next job call time
             * @return next job in list who's next call time matches incoming time, or NULL if no match found.
             */
            JobData * find_job(long long time ) ;

            /**
             * @brief Sets the next_job_call_time to the incoming time
             * @param in_time - the requested next job call time
             * @return always 0
             */
            int set_next_job_call_time(long long in_time ) ;

            /**
             * @brief Returns the next_job_time tracked while looping through jobs with find_next_job(long long)
             * @return next job call time in tics. Design [@ref d_schedqueue_getnextcalltime_0]
             */
            long long get_next_job_call_time() ;

            /**
             * @brief tests the current job next job call time against the current time and the overall next job call
             * time.  Sets the overall next job call time to the current job's time if it is lower
             * @return always 0
             */
            int test_next_job_call_time(Trick::JobData * curr_job, long long time_tics) ;

        private:

            /** number of jobs in list */
            unsigned int list_size ;

            /** Simple reallocable list of JobData pointers.  */
            JobData ** list ; /* ** This list is allocated outside of the memory manager. */

            /** current index to top job in list */
            unsigned int curr_index ;

            /** next lowest job call time as tracked by calls to find_next_job(long long) */
            long long next_job_time ;
    } ;

}

#endif

