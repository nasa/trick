
#include <iostream>
#include "trick/VariableServer.hh"
#include "trick/variable_server_sync_types.h"
#include "trick/TrickConstant.hh"

int Trick::VariableServer::get_next_freeze_call_time() {

    std::map < pthread_t , VariableServerThread * >::iterator it ;
    VariableServerThread * vst ;

    long long next_call_tics ;

    next_call_tics = TRICK_MAX_LONG_LONG ;

    pthread_mutex_lock(&map_mutex) ;
    for ( it = var_server_threads.begin() ; it != var_server_threads.end() ; it++ ) {
        vst = (*it).second ;
        if ( vst->get_freeze_next_tics() < next_call_tics ) {
            next_call_tics = vst->get_freeze_next_tics() ;
        }
    }
    pthread_mutex_unlock(&map_mutex) ;

    copy_data_freeze_job->next_tics = next_call_tics ;

    return(0) ;
}
