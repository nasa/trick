
#include <iostream>
#include <fstream>
#include <sstream>

#include "sim_services/Executive/include/Executive.hh"
#include "sim_services/Executive/include/Exec_exception.hh"
#include "sim_services/Executive/include/exec_proto.h"
#include "sim_services/include/release.h"

static bool isThreadReadyToRun( Trick::Threads * curr_thread , long long time_tics) {
    bool ret = false ;
    switch ( curr_thread->process_type ) {
        case Trick::PROCESS_TYPE_SCHEDULED:
            ret = true ;
            break ;
        case Trick::PROCESS_TYPE_AMF_CHILD:
            if ( curr_thread->amf_next_tics == time_tics ) {
                ret = true ;
            }
            break ;
        case Trick::PROCESS_TYPE_ASYNC_CHILD:
            if ( curr_thread->child_complete == true ) {
                if (curr_thread->amf_cycle_tics == 0 ) {
                    ret = true ;
                } else {
                    if ( curr_thread->amf_next_tics == time_tics ) {
                        ret = true ;
                    }
                }
            }
            break ;
    }
    return ret ;
}

/**
@details
-# Wait for all synchronous threads to finish initializing before entering infinite loop
-# Enter an infinite loop.  A call to exec_terminate(const char *) or
   exec_terminate_with_return(int, const char *, int , const char * ) will exit the loop.
    -# For all asynchronous must finish threads that are to finish at this time, wait for their completion
    -# If the exec_command equals freeze mode, call Trick::Executive::freeze_loop().  Transition from
       freeze back to run will continue execution at this point.
       Requirement  [@ref r_exec_mode_1]
    -# Set the job complete flags for all jobs to false.
    -# Check to see if all child threads are alive.  If a thread has exited, call
       Trick::Executive::exec_terminate_with_return(int, char *, int, char *)
       Requirement  [@ref r_exec_thread_7]
    -# Signal threads to start the next time step of processing.
    -# For each scheduled jobs whose next call time is equal to the current simulation time [@ref ScheduledJobQueue]
        -# Wait for all job dependencies to complete.  Requirement  [@ref r_exec_thread_6]
        -# Call the job.  Requirement  [@ref r_exec_periodic_0]
        -# If the job is a system job, check to see if the next job call time is the lowest next time by
           calling Trick::ScheduledJobQueue::test_next_job_call_time(Trick::JobData *, long long)
    -# If the exec_command equals ExitCmd
       -# Call Trick::Executive::exec_terminate_with_return(int, char *, int, char *)
    -# If the elapsed time has reached the termination time 
       -# Call Trick::Executive::exec_terminate_with_return(int, char *, int, char *)
    -# If the elapsed time equals the next software frame time 
       -# Call the end_of_frame jobs. Requirement  [@ref r_exec_periodic_2]
       -# Set the end of frame execution time to the current time + software_frame
*/
int Trick::Executive::loop_multi_thread() {

    JobData * depend_job ;
    unsigned int ii ;
    std::stringstream error_message ;
    Trick::ScheduledJobQueue * main_sched_queue ;
    int ret = 0 ;

    /* Wait for all threads to finish initializing and set the child_complete flag. */
    for (ii = 1; ii < threads.size() ; ii++) {
        Threads * curr_thread = threads[ii] ;
        while (curr_thread->child_complete == false ) {
            if (rt_nap == true) {
                RELEASE();
            }
        }
    }

    /* The main scheduler queue is the queue in thread 0 */
    main_sched_queue = &(threads[0]->job_queue) ;

    while (1) {

        /* Give aynchronous jobs to the top of the next time start to finish executing previous frame */
        for (ii = 0; ii < threads.size() ; ii++) {

            Trick::Threads * curr_thread = threads[ii] ;

            if ( curr_thread->process_type == PROCESS_TYPE_ASYNC_CHILD ) {
                if ( curr_thread->child_complete == true ) {
                    if (curr_thread->amf_cycle_tics != 0 ) {
                        // catch up async next_tic time to a time greater than the time last pass
                        while ( curr_thread->amf_next_tics < time_tics ) {
                            curr_thread->amf_next_tics += curr_thread->amf_cycle_tics ;
                        }
                    }
                }
            }
        }

        /* Call freeze_loop() if commanded by freeze() or a <CTRL-C> signal was caught. */
        if (exec_command == FreezeCmd) {
            exec_command = NoCmd;
            freeze_loop();
        }

        /* Call all top of frame jobs if the simulation time equals to the time software frame boundary. */
        if (time_tics == next_frame_check_tics - software_frame_tics ) {
            top_of_frame_queue.reset_curr_index() ;
            while ( (curr_job = top_of_frame_queue.get_next_job()) != NULL ) {
                ret = curr_job->call() ;
                if ( ret != 0 ) {
                    exec_terminate_with_return(ret , curr_job->name.c_str() , 0 , "top_of_frame job did not return 0") ;
                }
            }
            frame_count++ ;
        }

        /* Call the input_processor_run queue jobs. */
        input_processor_run_queue.reset_curr_index() ;
        while ( (curr_job = input_processor_run_queue.find_next_job( time_tics )) != NULL ) {
            ret = curr_job->call() ;
            if ( ret != 0 ) {
                exec_terminate_with_return(ret , curr_job->name.c_str() , 0 , "input_processor_run job did not return 0") ;
            }
            // input processor jobs set their own next job call time.  The test_next_job_call_time
            // will adjust the next call time for this queue
            input_processor_run_queue.test_next_job_call_time(curr_job , time_tics) ;

            /* System jobs next call time are not set until after they run. 
               Test their next job call time after they have been called */
            if ( curr_job->system_job_class ) {
                main_sched_queue->test_next_job_call_time(curr_job , time_tics) ;
            }
        }

        /* Go through all of the job queues and mark all jobs that are to run this time step to not complete. */
        for (ii = 0; ii < threads.size() ; ii++) {

            Trick::Threads * curr_thread = threads[ii] ;

            /* For all threads that are waiting to start the next cycle (child_complete == true)
               reset job completion flags */
            if ( isThreadReadyToRun(curr_thread, time_tics) ) {

                /* For all jobs in all threads that will run for this time_tic, */
                /* Set job complete flags to false.                             */
                /* The job complete flags are used for job depends_on checks.   */
                curr_thread->job_queue.reset_curr_index();
                while ( (curr_job = curr_thread->job_queue.find_job(time_tics)) != NULL ) {
                    curr_job->complete = false;
                }
            }
        }

        /* After all jobs on all threads that are going to run are set not complete, start the threads */
        for (ii = 1; ii < threads.size() ; ii++) {

            Trick::Threads * curr_thread = threads[ii] ;

            /* For all threads that are waiting to start the next cycle (child_complete == true)
               signal them to start through either the thread mutex or frame trigger. */
            if ( isThreadReadyToRun(curr_thread, time_tics) ) {

                curr_thread->curr_time_tics = time_tics ;
                curr_thread->child_complete = false ;
                curr_thread->amf_next_tics += curr_thread->amf_cycle_tics ;

                if (curr_thread->rt_semaphores == true ) {
                    pthread_mutex_lock(&(curr_thread->go_mutex));
                    pthread_cond_signal(&(curr_thread->go_cv));
                    pthread_mutex_unlock(&(curr_thread->go_mutex));
                } else {
                    curr_thread->frame_trigger = true ;
                }
            }
        }

        /* Get next job scheduled to run at the current simulation time step. */
        main_sched_queue->reset_curr_index() ;
        while ( (curr_job = main_sched_queue->find_next_job( time_tics )) != NULL ) {

            /* Wait for all jobs that the current job depends on to complete. */
            for ( ii = 0 ; ii < curr_job->depends.size() ; ii++ ) {
                depend_job = curr_job->depends[ii] ;
                while (! depend_job->complete) {
                    if (rt_nap == true) {
                        RELEASE();
                    }
                }
            }

            /* Call the current job scheduled to run at the current simulation time step. */
            ret = curr_job->call() ;
            if ( ret != 0 ) {
                exec_terminate_with_return(ret , curr_job->name.c_str() , 0 , "scheduled job did not return 0") ;
            }
            /* System jobs next call time are not set until after they run. 
               Test their next job call time after they have been called */
            if ( curr_job->system_job_class ) {
                main_sched_queue->test_next_job_call_time(curr_job , time_tics) ;
            }
            curr_job->complete = true ;
        }

        /* Call Executive::exec_terminate_with_return(int , const char * , int , const char *)
           if exec_command equals ExitCmd. */
        if (exec_command == ExitCmd) {
            exec_terminate_with_return( 0 , __FILE__ , __LINE__ , "Sim control Shutdown" ) ;
        }

        /* We are already in run... clear any redundant exec_commands to go to run */
        if ( exec_command == RunCmd ) {
            exec_command = NoCmd ;
        }

        /* Call Executive::exec_terminate_with_return( int , const char * , int , const char *)
           if simulation elapsed time has reached the termination time. */
        if (terminate_time <= time_last_pass_tics ) {
            time_tics = time_last_pass_tics ;
            exec_terminate_with_return( 0 , __FILE__ , __LINE__ , "Reached termination time" ) ;
        }

        /* Call all end of frame jobs if the simulation time equals to the time software frame boundary. */
        if (time_tics == next_frame_check_tics ) {
            end_of_frame_queue.reset_curr_index() ;
            while ( (curr_job = end_of_frame_queue.get_next_job()) != NULL ) {
                ret = curr_job->call() ;
                if ( ret != 0 ) {
                    exec_terminate_with_return(ret , curr_job->name.c_str() , 0 , "end_of_frame job did not return 0") ;
                }
            }

            next_frame_check_tics += software_frame_tics ;
        }

    }

    return(0) ;
}

