
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string.h>

#include "SnapFrameLog.hh"
#include "sim_services/FrameLog/include/FrameDataRecordGroup.hh"
#include "sim_services/Executive/include/exec_proto.hh"
#include "sim_services/Executive/include/exec_proto.h"
#include "sim_services/Clock/include/clock_proto.h"
#include "sim_services/DataRecord/include/data_record_proto.h"
#include "sim_services/CommandLineArguments/include/command_line_protos.h"
#include "sim_services/Message/include/message_proto.h"
#include "sim_services/ScheduledJobQueue/include/ScheduledJobQueueInstrument.hh"

//Constructor.
SnapFrameLog::SnapFrameLog() :
  userSpecJobList(&_userSpecJobList, "SnapFrameLog::JobDataItem")
{
    frame_log_flag = false ;
    drg_trick = NULL ;
    drg_frame = NULL ;

    time_value_attr.type = TRICK_LONG_LONG ;
    time_value_attr.size = sizeof(long long) ;
    time_value_attr.units = strdup("s") ;

}

SnapFrameLog::~SnapFrameLog() {
    free((char *)time_value_attr.units) ;
    // leaking all of the FrameDataRecordGroups
    // leaking timeline memory.
}

/**
@details

This routine is called during the "default_data" phase of initialization.  At that time
the number of threads in the sim will be known and this ensure that everything is
allocated before the input_processor is initially run.

-# Allocate a FrameDataRecordGroup to hold user job information on the main thread
-# Push the FrameDataRecordGroup to drg_users.
-# For each child thread in the sim
 -# Allocate a FrameDataRecordGroup to hold user job information on the child thread
 -# Push the FrameDataRecordGroup to drg_users.
-# Allocate a FrameDataRecordGroup to hold trick job information
-# Allocate a FrameDataRecordGroup to hold frame information
-# Allocate space for timeline for each thread.
*/
void SnapFrameLog::allocate_recording_groups() {

    Trick::FrameDataRecordGroup * fdrg ;
    int ii ;

    num_threads = exec_get_num_threads();
    /* Create log_user_jobs group and log_trick_jobs group to record job frame_times. */
    fdrg = new ("snap_userjobs_main") Trick::FrameDataRecordGroup(0, "snap_userjobs_main") ;

    // In the master/slave case, we still want the time taken by the main thread but FrameDataRecordGroup doesn't add
    // the jobs by default. So, we manually add it here.
    fdrg->add_job(0, 1000, (char *)"top_of_frame", NULL, 1.0, (char *)"start_timer", (char *)"TRK", 1) ;
    fdrg->add_job(0, 1001, (char *)"end_of_frame", NULL, 1.0, (char *)"stop_timer", (char *)"TRK", 65532) ;

    drg_users.push_back(fdrg) ;
    for ( ii = 1 ; ii < num_threads ; ii++ ) {
        std::ostringstream group_name ;
        group_name << "snap_userjobs_C" << ii ;
        fdrg = new (group_name.str().c_str()) Trick::FrameDataRecordGroup(ii, group_name.str()) ;
        drg_users.push_back(fdrg) ;
    }
    drg_trick = new ("snap_trick_jobs") Trick::FrameDataRecordGroup(0, "snap_trickjobs") ;
    drg_frame = new ("snap_frame") Trick::FrameDataRecordGroup(0, "snap_frame") ;

}

void SnapFrameLog::add_recording_vars_for_jobs() {

    unsigned int ii , dot ;
    REF2 * new_ref ;
    char job_name[256];
    std::vector<Trick::JobData *> all_jobs_vector ;

    if(userSpecJobList.stl.size() > 0) {
       jobVector.clear();
       addJob("real_time.rt_sync.rt_monitor");
       addJob("trick_sys.sched.advance_sim_time");
       for(unsigned int jj = 0; jj < userSpecJobList.stl.size(); ++jj) {
          JobDataItem & item = userSpecJobList.stl[jj];
          addJob( item );
       }
       all_jobs_vector = jobVector;
    } else {
       exec_get_all_jobs_vector(all_jobs_vector);
    }

    for ( ii = 0 ; ii < all_jobs_vector.size() ; ii++ ) {
        /** @li only makes sense to record frame time for scheduled, integ, and end_of_frame jobs */
        /** @li exclude system jobs because they are excluded in instrument_before/after */
        /** @li exclude frame_log jobs */
        if ( (all_jobs_vector[ii]->job_class >= exec_get_scheduled_start_index() ||
             (! all_jobs_vector[ii]->job_class_name.compare("integration")) ||
             (! all_jobs_vector[ii]->job_class_name.compare("derivative")) ||
             (! all_jobs_vector[ii]->job_class_name.compare("dynamic_event")) ||
             (! all_jobs_vector[ii]->job_class_name.compare("post_integration")) ||
             (! all_jobs_vector[ii]->job_class_name.compare("top_of_frame")) ||
             (! all_jobs_vector[ii]->job_class_name.compare("end_of_frame")))
           ) {
            dot = all_jobs_vector[ii]->name.find_first_of(".");
            if (!all_jobs_vector[ii]->name.compare(dot,11,".frame_log.")) {
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


            new_ref = (REF2 *)calloc(1 , sizeof(REF2)) ;
            /** @li add job frame id, job class and cycle time to displayed job name, and prepend "JOB_" so it stands out in quickplot */
            if ( all_jobs_vector[ii]->job_class >= exec_get_scheduled_start_index() ) {
                if ( all_jobs_vector[ii]->thread == 0 ) {
                    sprintf(job_name, "JOB_%s.%2.2f(%s_%2.3f)", all_jobs_vector[ii]->name.c_str(),
                        all_jobs_vector[ii]->frame_id, all_jobs_vector[ii]->job_class_name.c_str(), all_jobs_vector[ii]->cycle);
                } else { // add child thread # to name
                    sprintf(job_name, "JOB_%s_C%d.%2.2f(%s_%2.3f)",  all_jobs_vector[ii]->name.c_str(), all_jobs_vector[ii]->thread,
                        all_jobs_vector[ii]->frame_id, all_jobs_vector[ii]->job_class_name.c_str(), all_jobs_vector[ii]->cycle);
                }
            } else { // non-scheduled class
                sprintf(job_name, "JOB_%s.%2.2f(%s)", all_jobs_vector[ii]->name.c_str(),
                    all_jobs_vector[ii]->frame_id, all_jobs_vector[ii]->job_class_name.c_str());
            }
            // replace any colons in (C++) job name with underscores
            char *colon = strchr(job_name, ':') ;
            while (colon) {
                colon[0] = '#';
                colon = strchr(job_name, ':') ;
            }

            new_ref->reference = strdup(job_name);
            new_ref->address = &(all_jobs_vector[ii]->frame_time) ;
            new_ref->attr = &time_value_attr ;
            /** @li use TRK tag in S_define to identify trick jobs */
            // trick jobs
            if ( all_jobs_vector[ii]->tags.count("TRK") ) {
                trick_jobs.push_back(std::string(job_name));
                drg_trick->add_variable(new_ref) ;
                drg_trick->add_rec_job(all_jobs_vector[ii]) ;
                // save names of trick sim objects (ones that user can change) so we can color code jobs in DP file
                if (!all_jobs_vector[ii]->name.compare(dot,5,".drd.")) {
                    dr_sim_object_name = all_jobs_vector[ii]->parent_object->name;
                }
                if (!all_jobs_vector[ii]->name.compare(dot,8,".master.")) {
                    ms_sim_object_name = all_jobs_vector[ii]->parent_object->name;
                }
                if (!all_jobs_vector[ii]->name.compare(dot,9,".rt_sync.")) {
                    rt_sim_object_name = all_jobs_vector[ii]->parent_object->name;
                }
                //TODO: no message jobs yet - fill this in when messaging has a job to look at here
                // if () {
                    msg_sim_object_name = std::string("message");
                //}
            // user jobs
            } else {
                user_jobs.push_back(std::string(job_name));
                drg_users[all_jobs_vector[ii]->thread]->add_variable(new_ref) ;
                drg_users[all_jobs_vector[ii]->thread]->add_rec_job(all_jobs_vector[ii]) ;
            }
        }
    }

    /* set the recoring job be and end of frame job.  Set each recording group to it's proper thread. */
    std::vector< Trick::FrameDataRecordGroup *>::iterator fdrg_it ;
    for ( fdrg_it = drg_users.begin() , ii = 0 ; fdrg_it != drg_users.end() ; fdrg_it++ , ii++ ) {
        (*fdrg_it)->set_job_class("end_of_frame") ;
        (*fdrg_it)->set_phase(65533);
        (*fdrg_it)->set_thread(ii);
        if ( ii > 0 ) {
            std::ostringstream group_name ;
            group_name << "snap_userjobs_C" << ii ;
            (*fdrg_it)->add_variable( group_name.str() + ".frame_sched_time") ;
        } else {
            std::ostringstream group_name ;
            group_name << "snap_userjobs_main.frame_sched_time";
            (*fdrg_it)->add_variable( group_name.str()) ;
        }
    }
    drg_trick->set_job_class("end_of_frame") ;
    drg_trick->set_phase(65535);

}

/**
@details
-# Add the log_userjob frame time we created above to the log_frame group
-# Add the log_trickjob frame time we created above to the log_frame group
-# Set the recording job data_record_group.frame to end of frame -
   phase it last (after rt_monitor) because time set in rt_monitor
*/
void SnapFrameLog::add_recording_vars_for_frame() {

    REF2 * new_ref ;
    char job_name[256];
    int ii ;

    drg_frame->add_variable(rt_sim_object_name + std::string(".rt_sync.frame_sched_time")) ;
    drg_frame->add_variable(rt_sim_object_name + std::string(".rt_sync.frame_overrun_time")) ;

    /* add the log_userjob frame time we created above to the log_frame group */
    for ( ii = 0 ; ii < num_threads ; ii++ ) {
        new_ref = (REF2 *)calloc(1 , sizeof(REF2)) ;
        if (ii > 0) {
            sprintf(job_name, "JOB_data_record_group_snap_userjobs_C%d.data_record.%2.2f(end_of_frame)",
             ii,drg_users[ii]->write_job->frame_id);
        } else {
            sprintf(job_name, "JOB_data_record_group_snap_userjobs.data_record.%2.2f(end_of_frame)",
             drg_users[ii]->write_job->frame_id);
        }
        trick_jobs.push_back(std::string(job_name));
        new_ref->reference = strdup(job_name);
        new_ref->address = &(drg_users[ii]->write_job->frame_time);
        new_ref->attr = &time_value_attr ;
        drg_frame->add_variable(new_ref) ;
        drg_frame->add_rec_job(drg_users[ii]->write_job) ;
    }

    /* add the log_trickjob frame time we created above to the log_frame group */
    new_ref = (REF2 *)calloc(1 , sizeof(REF2)) ;
    sprintf(job_name, "JOB_data_record_group.trickjobs.%2.2f(end_of_frame)",drg_trick->jobs[0]->frame_id);
    trick_jobs.push_back(std::string(job_name));
    new_ref->reference = strdup(job_name);
    new_ref->address = &(drg_trick->write_job->frame_time);
    new_ref->attr = &time_value_attr ;
    drg_frame->add_variable(new_ref) ;
    drg_frame->add_rec_job(drg_trick->write_job) ;

    /* set the recording job data_record_group.frame to end of frame -
       phase it last (after rt_monitor) because time set in rt_monitor */
    drg_frame->set_job_class("end_of_frame") ;
    drg_frame->set_phase(65535);
}

/**
@details
-# For each FrameDataRecordGroup in drg_users
 -# Call add_data_record_group
-# Call add_data_record_group for drg_trick
-# Call add_data_record_group for drg_frame
*/
void SnapFrameLog::add_recording_groups_to_sim() {
    std::vector< Trick::FrameDataRecordGroup *>::iterator fdrg_it ;
    for ( fdrg_it = drg_users.begin() ; fdrg_it != drg_users.end() ; fdrg_it++ ) {
        add_data_record_group( *fdrg_it , Trick::DR_Ring_Buffer) ;
    }
    add_data_record_group(drg_trick, Trick::DR_Ring_Buffer) ;
    add_data_record_group(drg_frame, Trick::DR_Ring_Buffer) ;
}

/**
@details
-# For each FrameDataRecordGroup in drg_users
 -# Call remove_data_record_group
-# Call remove_data_record_group for drg_trick
-# Call remove_data_record_group for drg_frame
*/
void SnapFrameLog::remove_recording_groups_from_sim() {
    std::vector< Trick::FrameDataRecordGroup *>::iterator fdrg_it ;
    for ( fdrg_it = drg_users.begin() ; fdrg_it != drg_users.end() ; fdrg_it++ ) {
        remove_data_record_group( *fdrg_it) ;
    }
    remove_data_record_group(drg_trick) ;
    remove_data_record_group(drg_frame) ;
}

/**
@details
-# For each FrameDataRecordGroup in drg_users
 -# Call enable
-# Call enable for drg_trick
-# Call enable for drg_frame
*/
void SnapFrameLog::enable_recording_groups() {
    std::vector< Trick::FrameDataRecordGroup *>::iterator fdrg_it ;
    for ( fdrg_it = drg_users.begin() ; fdrg_it != drg_users.end() ; fdrg_it++ ) {
        (*fdrg_it)->enable() ;
    }
    drg_trick->enable() ;
    drg_frame->enable() ;
}

/**
@details
-# For each FrameDataRecordGroup in drg_users
 -# Call disable
-# Call disable for drg_trick
-# Call disable for drg_frame
*/
void SnapFrameLog::disable_recording_groups() {
    std::vector< Trick::FrameDataRecordGroup *>::iterator fdrg_it ;
    for ( fdrg_it = drg_users.begin() ; fdrg_it != drg_users.end() ; fdrg_it++ ) {
        (*fdrg_it)->disable() ;
    }
    drg_trick->disable() ;
    drg_frame->disable() ;
}

/**
@details
-# For each FrameDataRecordGroup in drg_users
 -# Call init
-# Call init for drg_trick
-# Call init for drg_frame
*/
void SnapFrameLog::init_recording_groups() {
    std::vector< Trick::FrameDataRecordGroup *>::iterator fdrg_it ;
    for ( fdrg_it = drg_users.begin() ; fdrg_it != drg_users.end() ; fdrg_it++ ) {
        (*fdrg_it)->init() ;
    }
    drg_trick->init() ;
    drg_frame->init() ;
}

/**
@details
-# Insert frame log clock jobs in queue before and after all jobs we want to log the time of.
*/
void SnapFrameLog::add_instrument_jobs() {
    if(jobVector.size() > 0) {
        Trick::JobData * snapBeforeJob = exec_get_job( "snap.frame_log.frame_clock_start", 1 );
        Trick::JobData * snapAfterob = exec_get_job( "snap.frame_log.frame_clock_stop", 1 );
        for(std::vector<Trick::JobData *>::iterator it = jobVector.begin(), end = jobVector.end(); it != end; ++it) {
            Trick::JobData * targetJob = *it;
            Trick::ScheduledJobQueueInstrument * snapBeforeJobInstr = new Trick::ScheduledJobQueueInstrument(snapBeforeJob, targetJob);
            Trick::ScheduledJobQueueInstrument * snapAfterJobInstr = new Trick::ScheduledJobQueueInstrument(snapAfterob, targetJob);
            targetJob->add_inst_before(snapBeforeJobInstr);
            targetJob->add_inst_after(snapAfterJobInstr);
        }
    } else {
        exec_instrument_before("snap.frame_log.frame_clock_start") ;
        exec_instrument_after("snap.frame_log.frame_clock_stop") ;
    }
}

/**
@details
-# Removes frame log clock jobs in queue before and after all jobs.
*/
void SnapFrameLog::remove_instrument_jobs() {
    if(jobVector.size() > 0) {
        for(std::vector<Trick::JobData *>::iterator it = jobVector.begin(), end = jobVector.end(); it != end; ++it) {
            Trick::JobData * targetJob = *it;
            targetJob->remove_inst( "snap.frame_log.frame_clock_start" );
            targetJob->remove_inst( "snap.frame_log.frame_clock_stop" );
        }
    } else {
        exec_instrument_remove("snap.frame_log.frame_clock_start") ;
        exec_instrument_remove("snap.frame_log.frame_clock_stop") ;
    }
}

//Instrumentation job to save job timeline start time.
int SnapFrameLog::frame_clock_start(Trick::JobData * curr_job ) {

    Trick::JobData * target_job = (Trick::JobData *)curr_job->sup_class_data ;

    /** @par Detailed Design: */
    if ( target_job != NULL ) {
        /** @li Set target job's start time. */
        target_job->rt_start_time = clock_time() ;
    }

    return(0) ;

}

//Instrumentation job to save job timeline stop time and frame time.
int SnapFrameLog::frame_clock_stop(Trick::JobData * curr_job) {

    Trick::JobData * target_job = (Trick::JobData *)curr_job->sup_class_data ;

    /** @par Detailed Design: */
    if ( target_job != NULL ) {
        if ( target_job->rt_start_time >= 0 ) {
            /** @li Set current job's stop time and frame time. */
            target_job->rt_stop_time = clock_time() ;
            target_job->frame_time += (target_job->rt_stop_time - target_job->rt_start_time);
        } else {
            target_job->frame_time = 0 ;
        }
    }

    return(0) ;

}

/**
@details
-# If we are enabled already, return
-# If the frame log data recording groups are not in the sim
 -# Add the frame log data recording groups to the sim
 -# Initialize the frame log data recording groups.
-# Add instrument jobs
-# Enable the recording groups
-# Set the frame log flag to true
*/
int SnapFrameLog::framelog_on() {

    if ( frame_log_flag == true ) {
        return(0) ;
    }
    // test if we have added the frame logging objects to the sim or not.
    // If we haven't add them now and initialize the data recording groups.
    // We do this in framelog_on so we don't add unnecessary jobs and create
    // log_rt files until we have to.
    if ( get_data_record_group(drg_trick->get_group_name()) == NULL ) {
        add_recording_groups_to_sim() ;
        init_recording_groups() ;
    }
    add_instrument_jobs() ;
    enable_recording_groups() ;
    frame_log_flag = true ;
    return(0) ;
}

/**
@details
-# If we are disabled already, return
-# Remove instrument jobs
-# Disable the recording groups
-# Set the frame log flag to false.
*/
int SnapFrameLog::framelog_off() {

    if ( frame_log_flag == false ) {
        return(0) ;
    }
    remove_instrument_jobs() ;
    disable_recording_groups() ;
    frame_log_flag = false ;
    return(0) ;
}

/**
@details
-# Allocate memory for all of the frame log recording groups.
-# Add variables for both frame and individual job recording groups.
-# Create DP files.
*/
void SnapFrameLog::default_data() {
    allocate_recording_groups() ;
    add_recording_vars_for_jobs() ;
    add_recording_vars_for_frame() ;
}

/**
@details

Clears any data_record information related to frame logging from the checkpoint reload.
This job must run before DataRecordGroup::restart.

DataRecordGroups save the list of variables it was recording when a checkpoint is taken.
This list is used by DataRecordGroups to restart data recording.
This list doesn't work with frame log groups because frame logging creates the
data record list of varaibles differently than normal recording groups.  So, we need
to erase the checkpointed list of frame log variables.  The correct variables will
be inserted during FrameLog::restart which is run after DataRecordGroup::restart.

-# For each FrameDataRecordGroup in drg_users
 -# Call clear_checkpoint_vars
-# Call clear_checkpoint_vars for drg_trick
-# Call clear_checkpoint_vars for drg_frame
*/
int SnapFrameLog::clear_data_record_info() {

    std::vector< Trick::FrameDataRecordGroup *>::iterator it ;
    for ( it = drg_users.begin() ; it != drg_users.end() ; it++ ) {
        (*it)->clear_checkpoint_vars() ;
    }
    drg_trick->clear_checkpoint_vars() ;
    drg_frame->clear_checkpoint_vars() ;

    return 0 ;
}

/**
@details

Restart job that restores framelogging on checkpoint restart.  This is run after
clear_data_record_info and the data record group restart jobs.

-# Add data recording varaibles for the jobs and frame
-# Add the frame start and stop instrument jobs
-# Call init_recording_groups().
*/
int SnapFrameLog::restart() {
    if ( frame_log_flag == true ) {
        add_recording_vars_for_jobs() ;
        add_recording_vars_for_frame() ;
        add_instrument_jobs() ;
        // We do not have to run add_recording_groups_to_sim because the sim
        // restart will have already done that.  We only need to re-init
        // the data recording groups.
        init_recording_groups() ;
    }
    return 0 ;
}

//Shutdown job that writes the job timeline data to disk and closes log files.
int SnapFrameLog::shutdown() {

    return(0) ;

}

void SnapFrameLog::addJob(std::string jobName, unsigned int instance) {
   userSpecJobList.stl.push_back( JobDataItem(jobName, instance) );
}

void SnapFrameLog::addJob( JobDataItem & item ) {
  Trick::JobData * job = exec_get_job( item.name.c_str(), item.instance_id );
  if( job != 0x0 )
  {
     jobVector.push_back( job );
  } else
  {
     std::stringstream ss;
     ss << "Could not find job " << item.name << " instance id = " << item.instance_id << " for JobDataTask";
     exec_terminate( __FUNCTION__, ss.str().c_str() );
  }
}
