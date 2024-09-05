#include "snapframelog.h"

#include <iomanip>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "sim_services/FrameLog/include/FrameLog.hh"
#include "sim_services/FrameLog/include/FrameDataRecordGroup.hh"
#include "sim_services/Executive/include/exec_proto.hh"
#include "sim_services/Executive/include/exec_proto.h"
#include "sim_services/Clock/include/clock_proto.h"
#include "sim_services/DataRecord/include/data_record_proto.h"
#include "sim_services/CommandLineArguments/include/command_line_protos.h"
#include "sim_services/Message/include/message_proto.h"
#include "sim_services/MemoryManager/include/memorymanager_c_intf.h"
#include "sim_services/ScheduledJobQueue/include/ScheduledJobQueueInstrument.hh"

//Constructor.
SnapFrameLog::SnapFrameLog()
{
    frame_log_flag = false;
    drg_trick = NULL;
    drg_frame = NULL;
    log_init_start = false;
    log_init_end = false;

    time_value_attr.type = TRICK_LONG_LONG;
    time_value_attr.size = sizeof(long long);
    time_value_attr.units = strdup("s");
}

SnapFrameLog::~SnapFrameLog()
{
    free((char *) time_value_attr.units);
}

// Save job start time.
int SnapFrameLog::log_start(Trick::JobData * curr_job)
{
    if (!log_init_start) {
        clock_reset(0);
        log_init_start = true;
    }

    Trick::JobData * job = (Trick::JobData *) curr_job->sup_class_data;
    if (job != NULL) {
        job->rt_start_time = clock_time();
    }

    //std::cout << "Frame logging on for " << job->name << std::endl;
    

    return (0);

}

// Save job stop time and frame time.
int SnapFrameLog::log_stop(Trick::JobData * curr_job)
{
    Trick::JobData * job = (Trick::JobData *) curr_job->sup_class_data;
    if (job != NULL) {
        if (job->rt_start_time >= 0) {
            job->frame_time += (clock_time() - job->rt_start_time);
        } else {
            job->frame_time = 0;
        }
    }

    return (0);

}

//Command to turn on frame logs, set up frame log data recording
int SnapFrameLog::log_on()
{
    std::vector < Trick::JobData * >all_jobs_vector;
    std::stringstream file_name;
    REF2 *new_ref;
    int ii, dot;
    char job_name[256];
    std::vector < std::string > trick_jobs;
    std::vector < std::string > user_jobs;
    Trick::FrameDataRecordGroup * fdrg;

    /** @par Detailed Design: */

    if (frame_log_flag == true) {
        return (0);
    }

    if(userSpecJobs.size() > 0) {
       addJob("real_time.rt_sync.rt_monitor");
       addJob("trick_sys.sched.advance_sim_time");
       all_jobs_vector = userSpecJobs;
    } else {
       exec_get_all_jobs_vector(all_jobs_vector);
    }
    num_threads = exec_get_num_threads();

    if (drg_frame == NULL) {

        /** @li Create log_user_jobs group and log_trick_jobs
                group to record job frame_times. */
        fdrg = new("snap_userjobs_main")
            Trick::FrameDataRecordGroup(0, "snap_userjobs_main");
        drg_users.push_back(fdrg);
        for (ii = 1; ii < num_threads; ii++) {
            std::ostringstream group_name;
            group_name << "snap_userjobs_C" << ii;
            fdrg =
                new(group_name.str().c_str())
                    Trick::FrameDataRecordGroup(ii, group_name.str());
            drg_users.push_back(fdrg);
        }
        drg_trick = new("snap_trick_jobs")
            Trick::FrameDataRecordGroup(0, "snap_trickjobs");
        for (ii = 0; ii < (int) all_jobs_vector.size(); ii++) {
            /** @li only makes sense to record frame time for scheduled,
                    integ, and end_of_frame jobs */
            /** @li exclude system jobs because they are excluded
                    in instrument_before/after */
            /** @li exclude frame_log jobs */
            if ((all_jobs_vector[ii]->job_class >=
                 exec_get_scheduled_start_index()
                 || (!all_jobs_vector[ii]->
                     job_class_name.compare("integration"))
                 || (!all_jobs_vector[ii]->job_class_name.compare("derivative"))
                 || (!all_jobs_vector[ii]->
                     job_class_name.compare("dynamic_event"))
                 || (!all_jobs_vector[ii]->
                     job_class_name.compare("post_integration"))
                 || (!all_jobs_vector[ii]->
                     job_class_name.compare("top_of_frame"))
                 || (!all_jobs_vector[ii]->
                     job_class_name.compare("end_of_frame")))
                ) {
                dot = all_jobs_vector[ii]->name.find_first_of(".");
                if (!all_jobs_vector[ii]->name.compare(dot, 11, ".frame_log.")) {
                    continue;
                }

                //
                // Since there are normally a bazillion SimBus jobs
                // which normally do not affect real-time - skip them
                // Do the same for obcs data exchange
                //
                if (!all_jobs_vector[ii]->name.compare(dot, 7, ".SimBus")) {
                    continue;
                }
                if (!all_jobs_vector[ii]->name.compare(0,8,"obcsData")) {
                    continue;
                }

                new_ref = (REF2 *) calloc(1, sizeof(REF2));
                /** @li add job frame id, job class and cycle time to
                        displayed job name, and prepend "JOB_" so it stands
                        out in quickplot */
                if (all_jobs_vector[ii]->job_class >=
                    exec_get_scheduled_start_index()) {
                    if (all_jobs_vector[ii]->thread == 0) {
                        sprintf(job_name,
                                "JOB_%s.%2.2f(%s_%2.3f)",
                                all_jobs_vector[ii]->name.c_str(),
                                all_jobs_vector[ii]->frame_id,
                                all_jobs_vector[ii]->job_class_name.c_str(),
                                all_jobs_vector[ii]->cycle);
                    } else {    // add child thread # to name
                        sprintf(job_name,
                                "JOB_%s_C%d.%2.2f(%s_%2.3f)",
                                all_jobs_vector[ii]->name.c_str(),
                                all_jobs_vector[ii]->thread,
                                all_jobs_vector[ii]->frame_id,
                                all_jobs_vector[ii]->job_class_name.c_str(),
                                all_jobs_vector[ii]->cycle);
                    }
                } else {        // non-scheduled class
                    sprintf(job_name, "JOB_%s.%2.2f(%s)",
                            all_jobs_vector[ii]->name.c_str(),
                            all_jobs_vector[ii]->frame_id,
                            all_jobs_vector[ii]->job_class_name.c_str());
                }
                // replace any colons in (C++) job name with underscores
                char *colon = strchr(job_name, ':');
                while (colon) {
                    colon[0] = '#';
                    colon = strchr(job_name, ':');
                }

                new_ref->reference = strdup(job_name);
                new_ref->address = &(all_jobs_vector[ii]->frame_time);
                new_ref->attr = &time_value_attr;
                /** @li use TRK tag in S_define to identify trick jobs */
                // trick jobs
                if (all_jobs_vector[ii]->tags.count("TRK")) {
                    trick_jobs.push_back(std::string(job_name));
                    drg_trick->add_variable(new_ref);
                    drg_trick->add_rec_job(all_jobs_vector[ii]);
                    // save names of trick sim objects
                    // (ones that user can change) so we can
                    // color code jobs in DP file
                    if (!all_jobs_vector[ii]->name.compare(dot, 9, ".rt_sync.")) {
                        rt_sim_object_name =
                            all_jobs_vector[ii]->parent_object->name;
                    }
                } else {
                    user_jobs.push_back(std::string(job_name));
                    drg_users[all_jobs_vector[ii]->
                              thread]->add_variable(new_ref);
                    drg_users[all_jobs_vector[ii]->
                              thread]->add_rec_job(all_jobs_vector[ii]);
                }
            }
        }
        /** @li set the recording job data_record_group.userjobs
                to end of frame */
        for (ii = 0; ii < num_threads; ii++) {
            drg_users[ii]->set_job_class("end_of_frame");
            drg_users[ii]->set_phase(65533);
            drg_users[ii]->set_thread(ii);
            if (ii > 0) {
                std::ostringstream group_name;
                group_name << "snap_userjobs_C" << ii;
                drg_users[ii]->add_variable(group_name.str() +
                                            ".frame_sched_time");
            }
            add_data_record_group(drg_users[ii], Trick::DR_Ring_Buffer);
        }

        /** @li set the recording job data_record_group.trickjobs
                to end of frame - phase it last (after rt_monitor)
                so we can log rt_monitor job */
        drg_trick->set_job_class("end_of_frame");
        drg_trick->set_phase(65534);
        add_data_record_group(drg_trick, Trick::DR_Ring_Buffer);

        /** @li Create log_frame group. */
        drg_frame = new Trick::FrameDataRecordGroup(0, "snap_frame");
        drg_frame->add_variable(rt_sim_object_name +
                                std::string(".rt_sync.frame_sched_time"));
        drg_frame->add_variable(rt_sim_object_name +
                                std::string(".rt_sync.frame_overrun_time"));

        /** @li add the log_userjob frame time we created
                above to the log_frame group */
        new_ref = (REF2 *) calloc(1, sizeof(REF2));
        sprintf(job_name,
                "JOB_data_record_group.userjobs.%2.2f(end_of_frame)",
                drg_users[0]->jobs[0]->frame_id);
        trick_jobs.push_back(std::string(job_name));
        new_ref->reference = strdup(job_name);
        new_ref->address = &(drg_users[0]->jobs[0]->frame_time);
        new_ref->attr = &time_value_attr;
        drg_frame->add_variable(new_ref);

        /** @li add the log_trickjob frame time we
                created above to the log_frame group */
        new_ref = (REF2 *) calloc(1, sizeof(REF2));
        sprintf(job_name,
                "JOB_data_record_group.trickjobs.%2.2f(end_of_frame)",
                drg_trick->jobs[0]->frame_id);
        trick_jobs.push_back(std::string(job_name));
        new_ref->reference = strdup(job_name);
        new_ref->address = &(drg_trick->jobs[0]->frame_time);
        new_ref->attr = &time_value_attr;
        drg_frame->add_variable(new_ref);

        /** @li set the recording job data_record_group.frame to end of frame -
               phase it last (after rt_monitor) because time set in rt_monitor */
        drg_frame->set_job_class("end_of_frame");
        drg_frame->set_phase(65534);
        add_data_record_group(drg_frame, Trick::DR_Ring_Buffer);
    }

    /** @li Insert frame log clock jobs in queue before and after all jobs
            we want to log the time of. */
    if(userSpecJobs.size() > 0) {
        Trick::JobData * snapBeforeJob = exec_get_job( "snap.frame.log_start", 1 );
        Trick::JobData * snapAfterob = exec_get_job( "snap.frame.log_stop", 1 );
        for(std::vector<Trick::JobData *>::iterator it = userSpecJobs.begin(), end = userSpecJobs.end(); it != end; ++it) {
            Trick::JobData * targetJob = *it;
            Trick::ScheduledJobQueueInstrument * snapBeforeJobInstr = new Trick::ScheduledJobQueueInstrument(snapBeforeJob, targetJob);
            Trick::ScheduledJobQueueInstrument * snapAfterJobInstr = new Trick::ScheduledJobQueueInstrument(snapAfterob, targetJob);
            targetJob->add_inst_before(snapBeforeJobInstr);
            targetJob->add_inst_after(snapAfterJobInstr);
        }
    } else {
      exec_instrument_before("snap.frame.log_start");
      exec_instrument_after("snap.frame.log_stop");
    }

    /** @li Turn frame log flag on. */
    frame_log_flag = true;
    return (0);

}

// Command to turn off frame logs.
int SnapFrameLog::log_off()
{

    /** @par Detailed Design: */
    int ii;

    if (frame_log_flag == false) {
        return (0);
    }

    drg_trick->disable();

    for (ii = 0; ii < num_threads; ii++) {
        drg_users[ii]->disable();
    }
    drg_frame->disable();
    exec_instrument_remove("snap.frame.log_start");
    exec_instrument_remove("snap.frame.log_stop");
    /** @li Turn frame log flag off. */
    frame_log_flag = false;

    return (0);

}

int SnapFrameLog::log_shutdown()
{
    return (0);
}

void SnapFrameLog::addJob(std::string jobName, unsigned int instance) {
  Trick::JobData * job = exec_get_job( jobName.c_str(), instance );
  if( job != 0x0 )
  {
     userSpecJobs.push_back( job );
  } else
  {
     std::stringstream ss;
     ss << "Could not find job " << jobName << " instance id = " << instance << " for JobDataTask";
     exec_terminate( __FUNCTION__, ss.str().c_str() );
  }
}
