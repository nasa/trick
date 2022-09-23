
#include <iostream>
#include <sys/time.h>

#include "trick/VariableServer.hh"
#include "trick/TrickConstant.hh"

int Trick::VariableServer::copy_data_freeze_scheduled() {

    long long next_call_tics ;
    VariableServerThread * vst ;
    std::map < pthread_t , VariableServerThread * >::iterator it ;

    next_call_tics = TRICK_MAX_LONG_LONG ;

    pthread_mutex_lock(&map_mutex) ;
    for ( it = var_server_threads.begin() ; it != var_server_threads.end() ; it++ ) {
        vst = (*it).second ;
        vst->copy_data_freeze_scheduled(copy_data_freeze_job->next_tics) ;
        if ( vst->get_freeze_next_tics() < next_call_tics ) {
            next_call_tics = vst->get_freeze_next_tics() ;
        }
    }
    pthread_mutex_unlock(&map_mutex) ;

    //reschedule the current job. TODO: a call needs to be created to do this the OO way
    if ( copy_data_freeze_job != NULL ) {
        copy_data_freeze_job->next_tics = next_call_tics ;
    }

    return(0) ;
}
