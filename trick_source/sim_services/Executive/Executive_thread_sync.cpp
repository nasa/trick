
#include <iostream>

#include "trick/Executive.hh"
#include "trick/release.h"

/**
@design
-# Loop through all threads
   -# If the thread is asynchronous must finish and the next sync time matches the sim time
      -# Wait for the thread to finish
      -# Reset the thread queue of jobs
      -# clear all job complete flags
   -# If the thread is asynchronous and the thread is finished
      -#  If the thread has a cycle time advance the next sync time beyond the current time
          by multiples of the cycle time.
      -# Reset the thread queue of jobs
      -# clear all job complete flags
*/

int Trick::Executive::thread_sync() {

    unsigned int ii ;

    /* Wait for async_must finish to complete at the current time_tics */
    for (ii = 1; ii < threads.size() ; ii++) {
        Threads * curr_thread = threads[ii] ;
        if ( (curr_thread->process_type == PROCESS_TYPE_AMF_CHILD) &&
              (curr_thread->amf_next_tics == time_tics )) {
            while (curr_thread->child_complete == false ) {
                if (rt_nap == true) {
                    RELEASE();
                }
            }
            curr_thread->job_queue.reset_curr_index();
            while ( (curr_job = curr_thread->job_queue.find_job(time_tics)) != NULL ) {
                curr_job->complete = false;
            }
        }
        else if ( curr_thread->process_type == PROCESS_TYPE_ASYNC_CHILD ) {
            if ( curr_thread->child_complete == true ) {
                if (curr_thread->amf_cycle_tics != 0 ) {
                    // catch up async next_tic time to a time greater than the time last pass
                    while ( curr_thread->amf_next_tics < time_tics ) {
                        curr_thread->amf_next_tics += curr_thread->amf_cycle_tics ;
                    }
                }
                curr_thread->job_queue.reset_curr_index();
                while ( (curr_job = curr_thread->job_queue.find_job(time_tics)) != NULL ) {
                    curr_job->complete = false;
                }
            }
        }
    }

    return(0) ;
}

