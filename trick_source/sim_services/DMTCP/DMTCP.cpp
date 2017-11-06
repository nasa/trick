
#include <iostream>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <fstream>
#include <iomanip>

#ifdef _DMTCP
#include "dmtcpaware.h"
#endif

#include "trick/DMTCP.hh"
#include "trick/SimObject.hh"
#include "trick/exec_proto.h"
#include "trick/message_proto.h"
#include "trick/Executive.hh"
#include "trick/CheckPointRestart.hh"

using namespace std;

Trick::DMTCP * the_dmtcp ;

Trick::DMTCP::DMTCP() {

    int num_classes = 0 ;

    checkpoint_now = false ;
    dmtcp_ckpt_complete = 0;

    class_map["dmtcp_pre"] = num_classes ;
    class_to_queue[num_classes++] = &pre_queue ;

    class_map["dmtcp_post"] = num_classes ;
    class_to_queue[num_classes++] = &post_queue ;

    class_map["dmtcp_restart"] = num_classes ;
    class_to_queue[num_classes++] = &restart_queue ;

    the_dmtcp = this ;
}

int Trick::DMTCP::init() {

#ifdef _DMTCP
    if( dmtcpIsEnabled() )
        dmtcpInstallHooks(dmtcp_pre_checkpoint, dmtcp_post_checkpoint, dmtcp_restart);
#endif

    call_dmtcp();

    return 0 ;
}


int Trick::DMTCP::freeze() {
    call_dmtcp();
    return 0;
}


int Trick::DMTCP::pre() {
    JobData * curr_job ;

    pre_queue.reset_curr_index() ;

    while ( (curr_job = pre_queue.get_next_job()) != NULL ) {
        curr_job->call() ;
    }
    return 0 ;
}

int Trick::DMTCP::post() {
    JobData * curr_job ;

    renameRestartScript();

    post_queue.reset_curr_index() ;

    while ( (curr_job = post_queue.get_next_job()) != NULL )
        curr_job->call() ;

    return 0 ;
}

int Trick::DMTCP::restart() {
    JobData * curr_job ;

    restart_queue.reset_curr_index() ;

    while ( (curr_job = restart_queue.get_next_job()) != NULL )
        curr_job->call() ;

    while ( !dmtcp_checkpoint_jobs_queue.empty())
        dmtcp_checkpoint_jobs_queue.pop();

    return 0 ;
}


int Trick::DMTCP::call_dmtcp() {

    if ( checkpoint_now == true ) {

        dmtcp_ckpt_complete = 0;
        sleep(1); // this sleep() is required to give flag
                  // enough time to refresh in Trick View
                  // it does not cause overruns

#ifdef _DMTCP
        if( dmtcpIsEnabled() )
            dmtcpRunCommand('c') ;
#endif
        checkpoint_now = false ;
    }
    return 0 ;
}

void Trick::DMTCP::dmtcpSetCheckpointNow() {
    checkpoint_now = true ;
}

std::string Trick::DMTCP::splitFilename( const string& str ) {
    size_t found;
    std::string filename;

    found=str.find_last_of("/\\");
    filename = str.substr(found+1);

    return filename;
}

bool Trick::DMTCP::isSpecialCharacter( const std::string& str) {

    size_t found = str.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_.-#%@=:^" );

    if (found!=string::npos)
        return true;
    else
        return false;
}

void  Trick::DMTCP::dmtcpJobQueue( std::string file_name ) {
    dmtcp_checkpoint_jobs_queue.push( file_name ) ;

    if ( (dmtcp_checkpoint_jobs_queue.size() >= 2) and (strcmp(file_name.c_str(),"dmtcp_chkpnt_safestore") ) )
        dmtcp_checkpoint_jobs_queue.pop() ;
}


std::string Trick::DMTCP::getScriptName() {
    std::string restart_script_name ;
#ifdef _DMTCP
    std::stringstream default_script_name ;

    // Default script name
    default_script_name << "dmtcp_chkpnt_" << std::fixed << std::setprecision(2) << exec_get_sim_time() ;

    // Script name from memory manager wrapper (input file or gui)
    std::string mmw_script_name = dmtcp_checkpoint_jobs_queue.top();

    // User specified a new directory
    if ( strstr(mmw_script_name.c_str(),"/") !=  NULL ){
        std::string stripped_name = splitFilename( mmw_script_name );
        std::cout << "DMTCP checkpoints are written to the --ckptdir directory (defined in $TRICK_HOME/bin/s_define_dmtcp).\n";
        mmw_script_name = stripped_name;
    }

    // User specifed a valid script name (or is using default script name)
    if ( ( mmw_script_name == default_script_name.str() ) or ( !isSpecialCharacter( mmw_script_name ) ) )
        restart_script_name = mmw_script_name;

    // User specifed an invalid script name
    else {
        std::cout << mmw_script_name.c_str() << " is not a valid name. The default DMTCP script name will be used: " << default_script_name.str() << endl;
        restart_script_name = default_script_name.str();
    }
#endif
    return restart_script_name;
}

void Trick::DMTCP::dmtcpRenameCmd() {
#ifdef _DMTCP
    std::stringstream dmtcp_rename_command;
    std::stringstream no_env_dmtcp_rename_command;
    std::stringstream trick_restart_script;
    std::stringstream dmtcp_restart_script;
    std::stringstream no_env_dmtcp_restart_script;
    std::string mmw_filename = getScriptName();

    const DmtcpLocalStatus * lsp = dmtcpGetLocalStatus() ;

    if (lsp->genId) {
    dmtcp_restart_script << lsp->env <<  "/dmtcp_restart_script_" << lsp->uniquePidStr << "_" << std::setfill('0') << std::setw(5) << lsp->genId << ".sh";
        no_env_dmtcp_restart_script << "./dmtcp_restart_script_" << lsp->uniquePidStr << "_" << std::setfill('0') << std::setw(5) << lsp->genId << ".sh";
    }
    else {
        dmtcp_restart_script << lsp->env <<  "/dmtcp_restart_script_" << lsp->uniquePidStr << ".sh";
        no_env_dmtcp_restart_script << "./dmtcp_restart_script_" << lsp->uniquePidStr << ".sh";
    }

    trick_restart_script << lsp->env << "/" << mmw_filename;
    dmtcp_rename_command << "mv " << dmtcp_restart_script.str() << " " << trick_restart_script.str();
    no_env_dmtcp_rename_command << "mv " << no_env_dmtcp_restart_script.str() << " " << trick_restart_script.str() << "; rm -f ./dmtcp_restart_script.sh";

    ifstream dmtcp_file( dmtcp_restart_script.str().c_str() );
    if ( dmtcp_file.good() ) {
    dmtcpSystemCmd( dmtcp_rename_command.str() );
    }

    // There is a bug in DMTCP that causes the DMTCP Coordinator (see dmtcp::DmtcpCoordinator::writeRestartScript())
    // to not recognize ENV_VAR_CHECKPOINT_DIR when dumping a checkpoint from a restarted checkpoint.
    // As a result, DMTCP writes the restart script to the current directory.
    // This code just checks to see if it's in the current dir, if so, move it to the ENV_VAR_CHECKPOINT_DIR with the new script name.
    ifstream no_env_dmtcp_file( no_env_dmtcp_restart_script.str().c_str() );
    if ( no_env_dmtcp_file.good() ) {
        dmtcpSystemCmd( no_env_dmtcp_rename_command.str() );
    }

    dmtcpSafestoreDir();

    // Don't print out "dumped" message until checkpont file (ckpt_*.dmtcp) is written
    std::stringstream dmtcp_temp;
    dmtcp_temp << lsp->env << "/ckpt_" << lsp->uniquePidStr << "_" << std::setfill('0') << std::setw(5) << lsp->genId << "/ckpt_S_main_" << getenv("TRICK_HOST_CPU") << ".exe_" << lsp->uniquePidStr << ".dmtcp.temp";

    while( access( dmtcp_temp.str().c_str(),F_OK ) == 0 ) {}

    dmtcp_ckpt_complete = 1;

    message_publish(MSG_INFO, "Dumped DMTCP Checkpoint: %s\n", mmw_filename.c_str()) ;

#endif
}

void Trick::DMTCP::dmtcpSystemCmd( const string& str ) {
    std::string real_system_name ;

    void* dlhandle ;
    void (*real_system_ptr)(const char *) = NULL ;

    dlhandle = dlopen( NULL, RTLD_LAZY) ;
    real_system_name = "_real_system" ;
    real_system_ptr = (void (*)(const char *))dlsym( dlhandle , real_system_name.c_str()) ;

    if ( real_system_ptr != NULL )
        (*real_system_ptr)( str.c_str() ) ;
    else
        system( str.c_str() );
}

void Trick::DMTCP::dmtcpSafestoreDir() {
#ifdef _DMTCP

    std::string restart_script_name;
    char* checkpoint_dirname ;
    char* long_checkpoint_name ;
    char rm_safestore_dir[512] ;
    const DmtcpLocalStatus * lsp = dmtcpGetLocalStatus() ;

    long_checkpoint_name = strdup(lsp->checkpointFilename) ;

    // For DMTCP Safestore checkpoints, only keep newest ckpt_* directory
    if ( restart_script_name.c_str() == "dmtcp_chkpnt_safestore" )  {

        checkpoint_dirname = dirname(long_checkpoint_name) ;

        // If there's a new ckpt_* directory, remove the previous version
        if ( strcmp(prev_checkpoint_dirname, checkpoint_dirname) ) {
            sprintf( rm_safestore_dir, "cd %s; rm -rf %s", lsp->env, prev_checkpoint_dirname);
            dmtcpSystemCmd( rm_safestore_dir );
        }

        strcpy( prev_checkpoint_dirname, checkpoint_dirname );
    }
#else
        strcpy( prev_checkpoint_dirname, "" );
#endif
}


void Trick::DMTCP::dmtcpCleanup() {
#ifdef _DMTCP
    char dmtcp_restart_script_sh[1024];
    const DmtcpLocalStatus * lsp = dmtcpGetLocalStatus() ;

    sprintf( dmtcp_restart_script_sh, "%s/dmtcp_restart_script.sh", lsp->env );
    unlink(dmtcp_restart_script_sh) ;

    if ( !dmtcp_checkpoint_jobs_queue.empty() )
        dmtcp_checkpoint_jobs_queue.pop();
#endif
}

void  Trick::DMTCP::renameRestartScript() {
#ifdef _DMTCP
    if( dmtcpIsEnabled() ) {
        dmtcpRenameCmd();
        dmtcpCleanup();
    }
#endif
}


int Trick::DMTCP::write_s_job_execution( FILE * fp ) {

    if ( fp == NULL ) {
        return(0) ;
    }

    fprintf(fp, "\n===================================================================================================\n") ;
    fprintf(fp, "DMTCP :\n\n") ;

    write_non_sched_queue(fp, &pre_queue) ;
    write_non_sched_queue(fp, &post_queue) ;
    write_non_sched_queue(fp, &restart_queue) ;

    return 0 ;
}

int Trick::DMTCP::instrument_job_before(Trick::JobData * instrument_job __attribute__((unused)) ) {
    return 0 ;
}
int Trick::DMTCP::instrument_job_after(Trick::JobData * instrument_job __attribute__((unused)) ) {
    return 0 ;
}
int Trick::DMTCP::instrument_job_remove(std::string in_job __attribute__((unused)) ) {
    return 0 ;
}
