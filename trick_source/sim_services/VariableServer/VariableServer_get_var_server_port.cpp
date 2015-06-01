
#include <stdio.h>
#include "trick/VariableServer.hh"

int Trick::VariableServer::create_tcp_socket(const char * address, unsigned short in_port ) {
    Trick::VariableServerListenThread * new_listen_thread = new Trick::VariableServerListenThread ;
    new_listen_thread->create_tcp_socket(address, in_port) ;
    new_listen_thread->copy_cpus(listen_thread.get_cpus()) ;
    new_listen_thread->create_thread() ;
    additional_listen_threads[new_listen_thread->get_pthread_id()] = new_listen_thread ;

    return 0 ;
}

int Trick::VariableServer::create_udp_socket(const char * address, unsigned short in_port ) {
    int ret ;
    Trick::VariableServerThread * vst ;
    vst = new Trick::VariableServerThread(NULL) ;
    ret = vst->create_udp_socket(address, in_port) ;
    if ( ret == 0 ) {
        vst->copy_cpus(listen_thread.get_cpus()) ;
        vst->create_thread() ;
    }
    //vst->var_debug(3) ;

    return ret ;
}

int Trick::VariableServer::create_multicast_socket(const char * mcast_address, const char * address, unsigned short in_port ) {
    int ret ;
    Trick::VariableServerThread * vst ;
    vst = new Trick::VariableServerThread(NULL) ;
    ret = vst->create_mcast_socket(mcast_address, address, in_port) ;
    if ( ret == 0 ) {
        vst->copy_cpus(listen_thread.get_cpus()) ;
        vst->create_thread() ;
    }
    //vst->var_debug(3) ;

    return ret ;
}

