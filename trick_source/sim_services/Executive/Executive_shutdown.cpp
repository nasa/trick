/*
   PURPOSE: (Graceful Simulation termination with timing data output.)

   PROGRAMMERS:
     (((Robert W. Bailey) (LinCom) (Feb 1991) (v1.0) (Initial Release.))
      ((Robert W. Bailey) (LinCom) (April 1992) (--) (Realtime clock))
      ((Scott Killingsworth) (LinCom) (September 1997) (--) (Issue #1016)))
 */

#include <stdlib.h>
#include <fcntl.h>
#include <iomanip>
#include <sys/resource.h>

#include "trick/Executive.hh"
#include "trick/ExecutiveException.hh"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include "trick/release.h"

/**
@design
-# Set the mode to Exit.
   Requirement  [@ref r_exec_mode_3]
-# Wait for asynchronous or AMF threads to complete if the user has specified to do so
-# The scheduler calls shutdown jobs
   Requirement  [@ref r_exec_discrete_4]
-# If an exception is caught during calling the shutdown jobs the exception is ignored as we are already terminating.
-# The scheduler prints a simulation shutdown message
-# The scheduler kills all child threads
-# Return the exception_return value.  (Set to 0 if no exception thrown.)
*/
int Trick::Executive::shutdown() {

    double sim_elapsed_time;
    double cpu_time;
    double actual_cpu_time;
    double sim_to_cpu;
    unsigned int ii;
    int process_id = 0 ;
    struct rusage cpu_usage_buf ;

    SIM_MODE prev_mode = mode ;

    /* Set mode to ExitMode. */
    mode = ExitMode;

    /*  If we made it past initialization */
    if ( prev_mode != Initialization ) {
        /* Wait for asynchronous threads to complete if the user has specified to wait. */
        for (ii = 1; ii < threads.size() ; ii++) {
        	if ( (threads[ii]->process_type == Trick::PROCESS_TYPE_ASYNC_CHILD || threads[ii]->process_type == Trick::PROCESS_TYPE_AMF_CHILD) && threads[ii]->shutdown_wait_async ) {
                message_publish(MSG_INFO, "Waiting for asynchronous or AMF thread %d to finish\n", ii ) ;
                while (threads[ii]->child_complete == false) {
                    RELEASE_1();
                }
            }
        }
    }

    try {

        /* Call the shutdown jobs. */
        if (initialization_complete == true) {
            shutdown_queue.reset_curr_index() ;
            while ( (curr_job = shutdown_queue.get_next_job()) != NULL ) {
                curr_job->call() ;
            }
        }
    } catch (Trick::ExecutiveException & ex ) {
        /* Handle exception type Trick::ExecutiveException.  Set the file name and error message.
           Return the exception return code. */
        except_return = ex.ret_code ;
        except_file += std::string(" then exception caught in ") + ex.file ;
        except_message += std::string(" then exception Message: ") + ex.message ;
    }

    getrusage(RUSAGE_SELF, &cpu_usage_buf);
    cpu_time = ((double) cpu_usage_buf.ru_utime.tv_sec) + ((double) cpu_usage_buf.ru_utime.tv_usec / 1000000.0);

    /* Calculate simulation elapsed sim time and actual cpu time */
    sim_elapsed_time = get_sim_time() - sim_start;
    actual_cpu_time = cpu_time - cpu_start;
    if (actual_cpu_time <= 1e-6) {
        sim_to_cpu = 1e8;
    } else {
        sim_to_cpu = sim_elapsed_time / actual_cpu_time;
    }

    /* Print a shutdown message. */
    message_publish(MSG_NORMAL , "\n"
            "SIMULATION TERMINATED IN\n"
            "  PROCESS: %d\n"
            "  ROUTINE: %s\n"
            "  DIAGNOSTIC: %s\n\n"
            "       SIMULATION START TIME: %12.3f\n"
            "        SIMULATION STOP TIME: %12.3f\n"
            "     SIMULATION ELAPSED TIME: %12.3f\n"
            "        ACTUAL CPU TIME USED: %12.3f\n"
            "       SIMULATION / CPU TIME: %12.3f\n"
            "     INITIALIZATION CPU TIME: %12.3f\n" ,
            process_id, except_file.c_str(), except_message.c_str() ,
            sim_start , get_sim_time() , sim_elapsed_time , actual_cpu_time , sim_to_cpu , cpu_init ) ;

    /* Kill all threads. */
    for (ii = 1; ii < threads.size() ; ii++) {
        if ( threads[ii]->running ) {
            pthread_cancel(threads[ii]->get_pthread_id()) ;
        }
#if ! ( __APPLE__ || __CYGWIN__ || __Lynx__ )
        // Even Linux is hanging on this call right now... hmmm.
        //pthread_join(threads[ii]->thread_id, (void **) NULL);
#endif
    }

    /* Return the exception_return value.  This defaults to 0 if no exceptions were thrown. */
    return(except_return) ;

}
