
#include <netdb.h>
#include "trick/VariableServer.hh"
#include "trick/tc_proto.h"

Trick::VariableServer * the_vs ;

Trick::VariableServer::VariableServer() :
 enabled(true) ,
 info_msg(false),
 log(false)
{
    the_vs = this ;
    pthread_mutex_init(&map_mutex, NULL);
}

Trick::VariableServer::~VariableServer() {
}

bool Trick::VariableServer::get_enabled() {
    return enabled ;
}

void Trick::VariableServer::set_enabled(bool on_off) {
    enabled = on_off ;
}

bool Trick::VariableServer::get_info_msg() {
    return info_msg ;
}

void Trick::VariableServer::set_var_server_info_msg_on() {
    info_msg = true;
}

void Trick::VariableServer::set_var_server_info_msg_off() {
    info_msg = false;
}

bool Trick::VariableServer::get_log() {
    return log ;
}

void Trick::VariableServer::set_var_server_log_on() {
    log = true;
    // turn log on for all current vs clients
    std::map < pthread_t , VariableServerThread * >::iterator it ;
    for ( it = var_server_threads.begin() ; it != var_server_threads.end() ; it++ ) {
        (*it).second->set_log_on();
    }
}

void Trick::VariableServer::set_var_server_log_off() {
    log = false;
    // turn log off for all current vs clients
    std::map < pthread_t , VariableServerThread * >::iterator it ;
    for ( it = var_server_threads.begin() ; it != var_server_threads.end() ; it++ ) {
        (*it).second->set_log_off();
    }
}

const char * Trick::VariableServer::get_hostname() {
    return (listen_thread.get_hostname()) ;
}

Trick::VariableServerListenThread & Trick::VariableServer::get_listen_thread() {
    return listen_thread ;
}

void Trick::VariableServer::add_vst(pthread_t in_thread_id, VariableServerThread * in_vst) {
    pthread_mutex_lock(&map_mutex) ;
    var_server_threads[in_thread_id] = in_vst ;
    pthread_mutex_unlock(&map_mutex) ;
}

Trick::VariableServerThread * Trick::VariableServer::get_vst(pthread_t thread_id) {
    std::map < pthread_t , Trick::VariableServerThread * >::iterator it ;
    Trick::VariableServerThread * ret = NULL ;
    pthread_mutex_lock(&map_mutex) ;
    it = var_server_threads.find(thread_id) ;
    if ( it != var_server_threads.end() ) {
        ret = (*it).second ;
    }
    pthread_mutex_unlock(&map_mutex) ;
    return ret ;
}

void Trick::VariableServer::delete_vst(pthread_t thread_id) {
    pthread_mutex_lock(&map_mutex) ;
    var_server_threads.erase(thread_id) ;
    pthread_mutex_unlock(&map_mutex) ;
}

void Trick::VariableServer::set_copy_data_job( Trick::JobData * in_job ) {
    copy_data_job = in_job ;
}

void Trick::VariableServer::set_copy_data_freeze_job( Trick::JobData * in_job ) {
    copy_data_freeze_job = in_job ;
}
