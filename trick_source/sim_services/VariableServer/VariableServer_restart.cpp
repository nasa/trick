
#include "trick/VariableServer.hh"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include "trick/tc_proto.h"

int Trick::VariableServer::restart() {
    listen_thread.restart() ;
    if ( listen_thread.get_pthread_id() == 0 ) {
        listen_thread.create_thread() ;
    }
    std::map < pthread_t , VariableServerListenThread * >::iterator it ;
    for( it = additional_listen_threads.begin() ; it != additional_listen_threads.end() ; it++ ) {
        (*it).second->restart() ;
    }
    return 0 ;
}

// =====================================================================================
// The following two functions work together to allow a MemoryManager (ASCII) checkpoint
// to be reloaded while a variable server client is connected.
// =====================================================================================

// Suspend variable server processing prior to reloading a checkpoint.
int Trick::VariableServer::suspendPreCheckpointReload() {
    std::map<pthread_t, VariableServerThread*>::iterator pos ;

    listen_thread.pause_listening() ;

    pthread_mutex_lock(&map_mutex) ;
    for ( pos = var_server_threads.begin() ; pos != var_server_threads.end() ; pos++ ) {
        VariableServerThread* vst = (*pos).second ;
        vst->preload_checkpoint() ;
    }
    pthread_mutex_unlock(&map_mutex) ;

    return 0;
}

// Resume variable server processing after reloading a MemoryManager (ASCII) checkpoint.
int Trick::VariableServer::resumePostCheckpointReload() {
    std::map<pthread_t, VariableServerThread*>::iterator pos ;

    pthread_mutex_lock(&map_mutex) ;
    // For each Variable Server Thread ...
    for ( pos = var_server_threads.begin() ; pos != var_server_threads.end() ; pos++ ) {
        VariableServerThread* vst = (*pos).second ;
        vst->restart() ;
    }
    pthread_mutex_unlock(&map_mutex) ;

    listen_thread.restart_listening() ;

    return 0;
}
