
#include <stdlib.h>
#include "trick/VariableServerThread.hh"
#include "trick/exec_proto.h"
#include "trick/TrickConstant.hh"

Trick::VariableServer * Trick::VariableServerThread::vs = NULL ;

Trick::VariableServerThread::VariableServerThread(TCDevice * in_listen_dev) :
 Trick::ThreadBase("VarServer") ,
 listen_dev(in_listen_dev) {

    debug = 0 ;
    enabled = true ;
    log = false ;
    connection_accepted = false ;
    conn_type = TCP ;
    copy_mode = VS_COPY_ASYNC ;
    write_mode = VS_WRITE_ASYNC ;
    frame_multiple = 1 ;
    frame_offset = 0 ;
    freeze_frame_multiple = 1 ;
    freeze_frame_offset = 0 ;
    binary_data = false;
    multicast = false;
    byteswap = false ;

    pause_cmd = false ;
    exit_cmd = false ;

    validate_address = false ;
    send_stdio = false ;

    update_rate = 0.1 ;
    cycle_tics = (long long)(update_rate * exec_get_time_tic_value()) ;

    next_tics = TRICK_MAX_LONG_LONG ;
    freeze_next_tics = TRICK_MAX_LONG_LONG ;

    connection.disabled = TC_COMM_FALSE ;
    connection.disable_handshaking = TC_COMM_TRUE ;
    connection.blockio_limit = 0.0 ;
    connection.blockio_type = TC_COMM_BLOCKIO ;
    connection.client_id = 0 ;
    connection.dmtcp_use_real = 1 ;
    strcpy(connection.client_tag, "") ;
    connection.error_handler = (TrickErrorHndlr *) calloc(1, (int)sizeof(TrickErrorHndlr));
    connection.error_handler->report_level = TRICK_ERROR_CAUTION;

    pthread_mutex_init(&copy_mutex, NULL);
    pthread_mutex_init(&restart_pause, NULL);

    var_data_staged = false;
    packets_copied = 0 ;

    incoming_msg = (char *) calloc(1, MAX_CMD_LEN);
    stripped_msg = (char *) calloc(1, MAX_CMD_LEN);

}

Trick::VariableServerThread::~VariableServerThread() {
    free( incoming_msg ) ;
    free( stripped_msg ) ;
}

void Trick::VariableServerThread::set_vs_ptr(Trick::VariableServer * in_vs) {
    vs = in_vs ;
}

// Command to turn on log to varserver_log file
int Trick::VariableServerThread::set_log_on() {
    log = true;
    return(0) ;
}

// Command to turn off log to varserver_log file
int Trick::VariableServerThread::set_log_off() {
    log = false;
    return(0) ;
}

long long Trick::VariableServerThread::get_next_tics() {
    if ( ! enabled ) {
        return TRICK_MAX_LONG_LONG ;
    }
    return next_tics ;
}

long long Trick::VariableServerThread::get_freeze_next_tics() {
    if ( ! enabled ) {
        return TRICK_MAX_LONG_LONG ;
    }
    return freeze_next_tics ;
}

Trick::VariableServer * Trick::VariableServerThread::get_vs() {
    return vs ;
}

TCDevice & Trick::VariableServerThread::get_connection() {
    return connection ;
}


