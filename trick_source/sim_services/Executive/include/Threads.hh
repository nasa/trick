/*
    PURPOSE:
        (Trick Threads implementation)
*/

/*
 * $Id: Threads.hh 3562 2014-06-18 16:32:31Z alin $
 */

#ifndef _THREAD_HH_
#define _THREAD_HH_

#include <queue>
#include <pthread.h>
#if __linux
#include <sys/types.h>
#endif
#include <unistd.h>
#include <semaphore.h>
#include <iostream>

#include "sim_services/ThreadBase/include/ThreadBase.hh"
#include "sim_services/SimObject/include/SimObject.hh"
#include "sim_services/ScheduledJobQueue/include/ScheduledJobQueue.hh"

namespace Trick {

    /** This is a list of all the available thread scheduling types */
    enum ProcessType {
        PROCESS_TYPE_SCHEDULED,          /**< Scheduled child */
        PROCESS_TYPE_ASYNC_CHILD,        /**< Asynchronous child */
        PROCESS_TYPE_AMF_CHILD,          /**< Asynchronous mustfinish child */
    } ;

    /**
     * One instance of this class is instantiated for each thread in the simulation.  This class
     * manages the scheduled jobs associated with it's thread.  It also manages the thread specific
     * realtime capabilities
     *
     * @author Robert W. Bailey
     * @author many other Trick developers of the past who did not add their names.
     * @author Alexander S. Lin
     *
     * @todo implement realtime controls rt_on and rt_off
     */
    class Threads : public Trick::ThreadBase {

        public:

            /**
             * This constructor initializes the member data and initializes the mutexes.
             */
            Threads( int in_id = 0 , bool rt_nap = true) ;

            /**
             * Sets the thread pthread ID.  Needed for the master thread that doesn't actually spawn a new thread.
             */
            void set_pthread_id( pthread_t in_pthread_id ) ;

            /**
             * Inherited from ThreadBase.  Executes the scheduler child thread.
             */
            virtual void * thread_body() ;

            /**
             * Sets the enabled flag
             * @param yes_no - yes or no
             */
            void set_enabled(bool yes_no) ;

            /**
             * Sets the process type from an incoming integer
             * @param in_process_type - incoming process type as an integer
             * @return 0 if successful, -1 if process_type does not exist
             */
            int set_process_type(int in_process_type) ;

            /**
             * Sets amf_cycle and calculates amf_cycle_tics
             * @param in_cycle - the incoming tics value
             * @return 0 if successful, -1 if process_type does not exist
             */
            int set_amf_cycle(double in_cycle , long long time_tic_value) ;

            /**
             * Adjusts amf_cycle_tics if the time tic changed
             * @param in_cycle - the incoming tics value
             * @return 0 if successful, -1 if process_type does not exist
             */
            int time_tic_changed(long long old_time_tic_value, long long time_tic_value) ;

            /** * Sets the wait at shutdown for asynchronous threads flag
             * @param yes_no - do we wait for the thread at shutdown or not?
             * @return always 0
             */
            int set_async_wait(int yes_no) ;

            /**
             * This job resets the scheduler queues during a checkpoint restart.
             * @return error code or 0 for no errors.
             */
            void clear_scheduled_queues() ;

            /**
             * Prints thread information to the incoming file pointer
             */
            virtual void dump( std::ostream & oss = std::cout ) ;

            /** Thread is enabled. */
            bool enabled ;                  /**< trick_units(--) */

            /** Thread index as specified in the S_define file. i.e. C1, C2, etc. */
            unsigned int thread_id;         /**< trick_units(--) */

            /** current time of executing jobs, could be different than time_tics for asynchronous threads */
            long long curr_time_tics ;      /**< trick_units(--) */

            /** Cycle time for aysnchronous_must_finish threads */
            double amf_cycle ;              /**< trick_units(s) */

            /** Cycle time for aysnchronous_must_finish threads */
            long long amf_cycle_tics;       /**< trick_units(--) */

            /** next time to call thread if this is an amf thread */
            long long amf_next_tics;        /**< trick_units(--) */

            /** Queue to hold scheduled jobs assigned to this thread. */
            Trick::ScheduledJobQueue job_queue ;  /**< trick_io(**) */

            /** Queue to hold AMF top of frame jobs.\n */
            Trick::ScheduledJobQueue top_of_frame_queue ; /**< trick_io(**) */

            /** Queue to hold AMF end of frame jobs.\n */
            Trick::ScheduledJobQueue end_of_frame_queue ; /**< trick_io(**) */

            /** Current job that is being run on this thread. */
            Trick::JobData * curr_job ;     /**< trick_io(**) */

            /** Copied parameter from executive to allow release of processor */
            bool rt_nap ;                   /**< trick_io(**) */

            /** Process type async, amf, sched */
            ProcessType process_type;       /**< trick_io(**) */

            /** Thread has completed all jobs for this time step */
            volatile bool child_complete;            /**< trick_io(**) */

            /** Trigger from master to start thread processing */
            volatile int frame_trigger;              /**< trick_units(--) */

            /** True if the thread was started without errors. */
            bool running;                   /**< trick_units(--) */

            /** Use a mutex to signal the start of thread processing */
            bool rt_semaphores;             /**< trick_units(--) */

            /** Condition variable to control start of thread processing */
            pthread_cond_t go_cv;           /**< trick_io(**) */

            /** Mutex to control start of thread processing */
            pthread_mutex_t go_mutex;       /**< trick_io(**) */

            /** Wait for asynchronous jobs to finish at shutdown */
            bool shutdown_wait_async;       /**< trick_units(--) */

    } ;

}

#endif

