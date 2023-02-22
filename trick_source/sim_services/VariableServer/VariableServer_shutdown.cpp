
#include "trick/VariableServer.hh"

int Trick::VariableServer::shutdown() {
    listen_thread.cancel_thread() ;
    for (const auto& listen_it : additional_listen_threads) {
        listen_it.second->cancel_thread();
    }

    pthread_mutex_lock(&map_mutex) ;
    for (const auto& it : var_server_threads) {
        it.second->cancel_thread() ;
    }
    pthread_mutex_unlock(&map_mutex) ;

    return 0 ;
}

