
#include <iostream>
#include <sys/time.h>

#include "trick/VariableServer.hh"

int Trick::VariableServer::copy_data_freeze() {

    std::map < pthread_t , VariableServerThread * >::iterator it ;

    pthread_mutex_lock(&map_mutex) ;
    for ( it = var_server_threads.begin() ; it != var_server_threads.end() ; it++ ) {
        (*it).second->copy_data_freeze() ;
    }
    pthread_mutex_unlock(&map_mutex) ;

    return 0 ;
}
