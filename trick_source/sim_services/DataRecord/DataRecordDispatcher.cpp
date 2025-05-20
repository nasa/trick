/*
   PURPOSE: (Copy data out of structures into data recording memory.)
   PROGRAMMERS:
     (((Robert W. Bailey) (LinCom Corp) (3/96) (SES upgrades))
      ((Alex Lin) (NASA) (6/02) (DR_changes upgrade)))
 */

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>

#include <signal.h>
#if __linux
#include <sys/syscall.h>
#endif

#include "trick/DataRecordDispatcher.hh"
#include "trick/exec_proto.h"
#include "trick/exec_proto.hh"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include "trick/command_line_protos.h"

Trick::DataRecordDispatcher * the_drd = NULL ;

Trick::DRDMutexes::DRDMutexes() {
    pthread_cond_init(&dr_go_cv, NULL);
    pthread_mutex_init(&dr_go_mutex, NULL);
    pthread_cond_init(&init_complete_cv, NULL);
    pthread_mutex_init(&init_complete_mutex, NULL);
    cancelled = false;
}

Trick::DRDWriterThread::DRDWriterThread(DRDMutexes & in_mutexes, std::vector <Trick::DataRecordGroup *> & in_groups) :
 SysThread("DR_Writer"),
 drd_mutexes(in_mutexes) ,
 groups(in_groups) {}

void * Trick::DRDWriterThread::thread_body() {
    pthread_mutex_lock(&(drd_mutexes.dr_go_mutex));

    /* tell the main thread that the writer is ready to go */
    pthread_mutex_lock(&(drd_mutexes.init_complete_mutex));
    pthread_cond_signal(&(drd_mutexes.init_complete_cv));
    pthread_mutex_unlock(&(drd_mutexes.init_complete_mutex));

    /* from now until death, wait for the condition variable,
       then call the write_data method for all of the groups */
    while(1) {
        pthread_cond_wait(&(drd_mutexes.dr_go_cv), &(drd_mutexes.dr_go_mutex));
        if (drd_mutexes.cancelled) {
            pthread_mutex_unlock(&(drd_mutexes.dr_go_mutex));
            pthread_exit(0);
        }
        for ( unsigned int ii = 0 ; ii < groups.size() ; ii++ ) {
            if ( groups[ii]->buffer_type == Trick::DR_Buffer ) {
                groups[ii]->write_data(true) ;
            }
        }
    }
    pthread_mutex_unlock(&(drd_mutexes.dr_go_mutex));
    return NULL ;
}

void Trick::DRDWriterThread::dump( std::ostream & oss ) {
    oss << "Trick::DRDWriterThread (" << name << ")" << std::endl ;
    oss << "    number of data record groups = " << groups.size() << std::endl ;
    Trick::ThreadBase::dump(oss) ;
}

Trick::DataRecordDispatcher::DataRecordDispatcher()
    : drd_writer_thread(drd_mutexes, groups),
      verify_log_vars(true)
{
    the_drd = this;
}

Trick::DataRecordDispatcher::~DataRecordDispatcher() {
}

int Trick::DataRecordDispatcher::remove_files() {

    std::string command;
    command = std::string("/bin/rm -rf ") + command_line_args_get_output_dir() + std::string("/log_*") ;
    system(command.c_str());
    return 0 ;
}

/**
@details
-# Initialize thread mutex and condition variable
-# Create a new thread calling the DataRecordThreaded Writer routine.
-# Wait for the data record thread to initialize before continuing.
*/
int Trick::DataRecordDispatcher::init() {

    pthread_mutex_lock(&drd_mutexes.init_complete_mutex);
    drd_writer_thread.create_thread() ;
    pthread_cond_wait(&drd_mutexes.init_complete_cv, &drd_mutexes.init_complete_mutex);
    pthread_mutex_unlock(&drd_mutexes.init_complete_mutex);

    return(0) ;
}

/**
add_sim_object is called by the executive when a new sim_object is added to the sim.
@details
-# Test if the incoming object is a DataRecordGroup.
 -# If it is add it to the list of groups we save
*/
int Trick::DataRecordDispatcher::add_sim_object(Trick::SimObject * in_object ) {
    Trick::DataRecordGroup * drg = dynamic_cast< Trick::DataRecordGroup * >(in_object) ;
    if ( drg != NULL ) {
        groups.push_back(drg) ;
    }
    return 0 ;
}

/** @brief set verification check on/off */
void Trick::DataRecordDispatcher::set_verif_onoff(bool on)
{
    verify_log_vars = on;
}

/**
@details
-# Test if the incoming data record group name has been used.
 -# If true return an error.
-# Set the group buffering type to the incoming type
-# Add the group to the executive.
*/
int Trick::DataRecordDispatcher::add_group(Trick::DataRecordGroup * in_group, Trick::DR_Buffering buffering) {

    unsigned int ii ;
    for (  ii = 0 ; ii < groups.size() ; ii++ ) {
        if ( !groups[ii]->group_name.compare(in_group->group_name) ) {
            message_publish(MSG_ERROR, "Data Record group with duplicate name %s.\n", in_group->group_name.c_str()) ;
            return -1 ;
        }
    }
    if ( buffering != Trick::DR_Not_Specified ) {
        in_group->set_buffer_type(buffering) ;
    }

    exec_add_sim_object(in_group , (char *)in_group->name.c_str()) ;

    return 0 ;
}


/**
@details
-# Remove the data recording group from the dispatcher's list of groups
-# Remove the group from the executive.
*/
int Trick::DataRecordDispatcher::remove_group(Trick::DataRecordGroup * in_group) {

    std::vector <Trick::DataRecordGroup *>::iterator drg_it ;


    // remove the group from the dispatcher vector of jobs.
    for ( drg_it = groups.begin() ; drg_it != groups.end() ; ) {
        if ( (*drg_it) == in_group ) {
            // erase the group from the dispatcher. Lock the mutex so we aren't in the
            // middle of writing data as we delete the group.
            pthread_mutex_lock(&drd_mutexes.dr_go_mutex) ;
            drg_it = groups.erase(drg_it) ;
            pthread_mutex_unlock(&drd_mutexes.dr_go_mutex) ;

            // call exec_remove_sim_object to remove the data recording jobs from the sim.
            exec_remove_sim_object(in_group) ;

            // shutdown recording for the group
            in_group->shutdown() ;
        } else {
            drg_it++ ;
        }
    }



    return 0 ;
}

void Trick::DataRecordDispatcher::remove_all_groups() {
    while (!groups.empty()) {
        remove_group(groups[0]);
    }
}

/**
 @details
 -# Gets the data recording group by its name
 */
Trick::DataRecordGroup * Trick::DataRecordDispatcher::get_group(std::string in_name) {
    std::vector <Trick::DataRecordGroup *>::iterator it ;
    for ( it = groups.begin() ; it != groups.end() ; ++it ) {
        if ( ! (*it)->get_group_name().compare(in_name) )
            return *it ;
    }
    return NULL ;
}

/**
 @details
 -# Gets the data recording group by its id number
 */
Trick::DataRecordGroup * Trick::DataRecordDispatcher::get_group(int in_idx) {
    if (!groups.empty() && in_idx > -1 && in_idx < groups.size()) {
        return groups[in_idx];
    }
    return NULL ;
}

/**
 @details
 -# Gets the size of all added data recroding groups
 */
int Trick::DataRecordDispatcher::get_groups_size() {
    return groups.size();
}

/**
@details
-# If the writer thread condition variable is unlocked
   -# Signal the thread to go
*/
int Trick::DataRecordDispatcher::signal_thread() {

    if (!pthread_mutex_trylock(&drd_mutexes.dr_go_mutex)) {
        pthread_cond_signal(&drd_mutexes.dr_go_cv);
        pthread_mutex_unlock(&drd_mutexes.dr_go_mutex);
    }

    return(0) ;
}

/**
@details
-# Close out current data record groups
-# Clears the data record groups we are tracking.  The checkpoint reload will
   repopluate this list.
*/
int Trick::DataRecordDispatcher::preload_checkpoint() {
    unsigned int ii ;
    // close out current data record groups
    for ( ii = 0 ; ii < groups.size() ; ii++ ) {
        groups[ii]->shutdown() ;
    }
    groups.clear() ;
    return 0 ;
}

/**
@details
-# If we have not called init yet (pthread_id == 0), call init.
-# Delete existing log files.
*/
int Trick::DataRecordDispatcher::restart() {
    // if we restarted from scratch without calling init jobs, need to call init
    if ( drd_writer_thread.get_pthread_id() == 0 ) {
        init() ;
    }
    // remove the current log files
    remove_files() ;
    return 0 ;
}

/**
@details
-# If the thread was started,
   -# Wait for the thread to be available
   -# Cancel the thread
*/
int Trick::DataRecordDispatcher::shutdown() {

    if ( drd_writer_thread.get_pthread_id() != 0 ) {
        pthread_mutex_lock( &drd_mutexes.dr_go_mutex);
        // pthread_cancel( drd_writer_thread.get_pthread_id()) ;
        drd_mutexes.cancelled = true;
        pthread_cond_signal(&drd_mutexes.dr_go_cv);
        pthread_mutex_unlock( &drd_mutexes.dr_go_mutex);
    }

    return(0) ;
}

int Trick::DataRecordDispatcher::enable( const char * in_name ) {
    unsigned int ii ;
    for ( ii = 0 ; ii < groups.size() ; ii++ ) {
        if ( in_name == NULL or !groups[ii]->get_group_name().compare(in_name) )
            groups[ii]->enable() ;
    }
    return 0 ;
}

int Trick::DataRecordDispatcher::disable( const char * in_name ) {
    unsigned int ii ;
    for ( ii = 0 ; ii < groups.size() ; ii++ ) {
        if ( in_name == NULL or !groups[ii]->get_group_name().compare(in_name) )
            groups[ii]->disable() ;
    }
    return 0 ;
}

int Trick::DataRecordDispatcher::record_now_group( const char * in_name ) {
    unsigned int ii ;
    if ( in_name != NULL ) {
        for ( ii = 0 ; ii < groups.size() ; ii++ ) {
            if ( !groups[ii]->get_group_name().compare(in_name) )
                groups[ii]->data_record(exec_get_sim_time()) ;
        }
    }
    return 0 ;
}

int Trick::DataRecordDispatcher::set_group_max_file_size(const char * in_name, uint64_t bytes){
    unsigned int ii ;
    for ( ii = 0 ; ii < groups.size() ; ii++ ) {
        if ( in_name == NULL or !groups[ii]->get_group_name().compare(in_name) )
            groups[ii]->set_max_file_size(bytes) ;
    }
    return 0 ;
}

int Trick::DataRecordDispatcher::set_max_file_size(uint64_t bytes) {
    unsigned int ii ;
    for ( ii = 0 ; ii < groups.size() ; ii++ ) {
        groups[ii]->set_max_file_size(bytes) ;
    }
    return 0 ;
}

/**
@details
-# Call every group's init job - only needed when restoring checkpoint
-# during initialization, and the user is responsible for calling this.
*/
int Trick::DataRecordDispatcher::init_groups() {
    unsigned int ii ;
    for ( ii = 0 ; ii < groups.size() ; ii++ ) {
        groups[ii]->init() ;
    }

    if(verify_log_vars)
    {
        processMultipleVarLoggedCheck();
    }

    return 0 ;
}

/**
@details
 * Checks the data logging configuration in Trick against certain error or warning conditions.
 * Current conditions for errors are:
 * -  A variable logged in two different DataRecordGroup instances at a different rate
 */
void Trick::DataRecordDispatcher::processMultipleVarLoggedCheck()
{
    bool isLoggedMultipleTimes = false;

    // Check that a variable isn't logged multiple times in a log file.
    checkMultiVarSingleLogGroup(isLoggedMultipleTimes);

    // Check that a variable isn't logged in multiple log files.
    checkMultiVarMultiLogGroups(isLoggedMultipleTimes);

    if(isLoggedMultipleTimes)
    {
        std::stringstream ss;
        ss << "Invalid or unsafe logging configuration. Exiting..." << std::endl;
        exec_terminate_with_return(-1, __FILE__, __LINE__, ss.str().c_str());
    }
}

void Trick::DataRecordDispatcher::checkMultiVarSingleLogGroup(bool & isLoggedMultipleTimes)
{
    // Check that a variable isn't logged multiple times in a log file.
    for(size_t grpIdx = 0; grpIdx < groups.size(); ++grpIdx)
    {
        Trick::DataRecordGroup * drGroup = groups[grpIdx];
        // Start at index 1 to skip exec time.
        for(size_t varOneIdx = 1; varOneIdx < drGroup->rec_buffer.size(); ++varOneIdx)
        {
            for(size_t varTwoIdx = varOneIdx + 1; varTwoIdx < drGroup->rec_buffer.size(); ++varTwoIdx)
            {
                Trick::DataRecordBuffer * var1 = drGroup->rec_buffer[varOneIdx];
                Trick::DataRecordBuffer * var2 = drGroup->rec_buffer[varTwoIdx];
                const std::string & var1CmpStr = var1->alias.empty() ? var1->name : var1->alias;
                const std::string & var2CmpStr = var2->alias.empty() ? var2->name : var2->alias;
                if(var1CmpStr.compare(var2CmpStr) == 0)
                {
                    isLoggedMultipleTimes = true;
                    // ERROR: OUR EXPERIENCE HERE IS THIS SCREWS UP DR PRODUCTS ENOUGH TO CONSIDER THIS FATAL
                    std::stringstream ss;
                    ss << "The variable \"";
                    if(var1->alias.empty())
                    {
                        ss << var1->name << "\"";
                    }
                    else
                    {
                        ss << var1->name << "\" (logged as alias \"" << var1->alias << "\")";
                    }
                    ss << " is being logged twice in the data recording group \"" << drGroup->group_name << "\" ."
                       << std::endl;
                    message_publish(MSG_ERROR, ss.str().c_str());
                }
            }
        }
    }
}

void Trick::DataRecordDispatcher::checkMultiVarMultiLogGroups(bool & isLoggedMultipleTimes)
{
    for(size_t grpOneIdx = 0; grpOneIdx < groups.size(); ++grpOneIdx)
    {
        Trick::DataRecordGroup * drGroupOne = groups[grpOneIdx];
        for(auto & var1 : drGroupOne->rec_buffer)
        {
            if(var1->name.compare("sys.exec.out.time") == 0)
            {
                continue;
            }
            for(size_t grpTwoIdx = grpOneIdx + 1; grpTwoIdx < groups.size(); ++grpTwoIdx)
            {
                Trick::DataRecordGroup * drGroupTwo = groups[grpTwoIdx];

                auto nameCompare = [var1](Trick::DataRecordBuffer * var2)
                {
                    const std::string & var1CmpStr = var1->alias.empty() ? var1->name : var1->alias;
                    const std::string & var2CmpStr = var2->alias.empty() ? var2->name : var2->alias;
                    return (var1CmpStr.compare(var2CmpStr) == 0);
                };

                auto result = std::find_if(drGroupTwo->rec_buffer.begin(), drGroupTwo->rec_buffer.end(), nameCompare);
                while(result != drGroupTwo->rec_buffer.end())
                {
                    isLoggedMultipleTimes = true;
                    // ERROR: OUR EXPERIENCE HERE IS THIS SCREWS UP DR PRODUCTS ENOUGH TO CONSIDER THIS FATAL
                    std::stringstream ss;
                    ss << "The variable \"";
                    if(var1->alias.empty())
                    {
                        ss << var1->name << "\"";
                    }
                    else
                    {
                        ss << var1->name << "\" (logged as alias \"" << var1->alias << "\")";
                    }
                    ss << " is being logged in both \"" << drGroupOne->group_name << "\" and \""
                       << drGroupTwo->group_name << "\" data recording groups." << std::endl;
                    message_publish(MSG_ERROR, ss.str().c_str());
                    result = std::find_if(++result, drGroupTwo->rec_buffer.end(), nameCompare);
                }
            }
        }
    }
}
