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
#include <iostream>
#include <sys/resource.h>
#include "trick/Executive.hh"
#include "trick/ExecutiveException.hh"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include "trick/release.h"
#include "trick/SysThread.hh"

#ifdef PROFILE
#include <algorithm>
typedef struct {
    Trick::JobData* job;
    double total_time;
    double avg_time;
    unsigned long long call_time;
} JobProfile;

typedef struct {
    Trick::SimObject* obj; 
    double total_time;
    std::vector<JobProfile> jobProfiles;
} ObjProfile;

bool sortJobs(JobProfile j, JobProfile k) { return j.total_time < k.total_time; }

bool sortObjs(ObjProfile o, ObjProfile p) { return o.total_time < p.total_time; }
#endif

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
    double sim_mem;
    
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

    Trick::SysThread::ensureAllShutdown();

    getrusage(RUSAGE_SELF, &cpu_usage_buf);
    cpu_time = ((double) cpu_usage_buf.ru_utime.tv_sec) + ((double) cpu_usage_buf.ru_utime.tv_usec / 1000000.0);
    
    /* Get memory usage in MB for the calling process. Note that ru_maxrss returns long value in bytes on Mac and kilobytes on Linux. */
    #if __APPLE__ 
        sim_mem = (double)cpu_usage_buf.ru_maxrss / (1024 * 1024);
    #else 
        sim_mem = (double)cpu_usage_buf.ru_maxrss / 1024;
    #endif

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
            "     INITIALIZATION CPU TIME: %12.3f\n" 
            "        SIMULATION RAM USAGE: %12.3fMB\n"
            "  (External program RAM usage not included!)\n",
            process_id, except_file.c_str(), except_message.c_str() ,
            sim_start , get_sim_time() , sim_elapsed_time , actual_cpu_time , sim_to_cpu , cpu_init, sim_mem ) ;


    
#ifdef PROFILE
    double total_job_time;
    std::vector<ObjProfile> objProfiles;
    for(Trick::SimObject* s : sim_objects) {
        
        ObjProfile op;
        op.obj = s;
        op.total_time = 0;
        for(Trick::JobData* j : s->jobs) {
            JobProfile jp;
            jp.job = j;
            jp.call_time = j->call_times.size();
            jp.total_time = 0;
            for(double d : j->call_times) 
                jp.total_time += d;
            
            jp.avg_time = jp.total_time/jp.call_time;
            op.jobProfiles.push_back(jp);
            op.total_time += jp.total_time;
            
        }
        total_job_time += op.total_time;
        std::sort(std::begin(op.jobProfiles), std::end(op.jobProfiles), sortJobs);
        objProfiles.push_back(op);
    }
        
    std::sort(std::begin(objProfiles), std::end(objProfiles), sortObjs);
            

    FILE* profileFile = fopen("JobProfile.csv", "w");
    
    fprintf(profileFile, "Name,Total Runtime, Runtime Percentage, Call Count, Average Time\n");
    for(ObjProfile o : objProfiles) {
        fprintf(profileFile, "\n%s, %.4f, %.4f\n", o.obj->name.data(), o.total_time, 100 * (o.total_time/total_job_time));
        for(JobProfile j : o.jobProfiles)  {
            if(j.call_time)
                fprintf(profileFile, "%s, %.4f, %.4f, %llu, %.4f\n", j.job->name.data(), j.total_time, 100 * (j.total_time/total_job_time), j.call_time, j.avg_time);
            else
                fprintf(profileFile, "%s, %.4f, %.4f, 0, 0.0000\n", j.job->name.data(), j.total_time, 100 * (j.total_time/total_job_time));
        }
            
        
        
    }
    fclose(profileFile);
#endif
    
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
