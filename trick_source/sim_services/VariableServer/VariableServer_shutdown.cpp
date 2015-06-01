
#include "trick/VariableServer.hh"

int Trick::VariableServer::shutdown() {
    listen_thread.cancel_thread() ;
    std::map < pthread_t , VariableServerThread * >::iterator it ;
    for ( it = var_server_threads.begin() ; it != var_server_threads.end() ; it++ ) {
        (*it).second->cancel_thread() ;
        // cancelling causes each var_server_thread map element to be erased by the exit_var_thread function
    }

    return 0 ;
}

