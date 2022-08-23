#include "trick/VariableServerSession.hh"
#include "trick/TrickConstant.hh"
#include "trick/exec_proto.h"

Trick::VariableServerSession::VariableServerSession(TCDevice * conn) {
    debug = 0 ;
    enabled = true ;
    log = false ;
    copy_mode = VS_COPY_ASYNC ;
    write_mode = VS_WRITE_ASYNC ;
    frame_multiple = 1 ;
    frame_offset = 0 ;
    freeze_frame_multiple = 1 ;
    freeze_frame_offset = 0 ;
    update_rate = 0.1 ;
    cycle_tics = (long long)(update_rate * exec_get_time_tic_value()) ;

    next_tics = TRICK_MAX_LONG_LONG ;
    freeze_next_tics = TRICK_MAX_LONG_LONG ;
    binary_data = false;
    // multicast = false;
    connection = conn;
    byteswap = false ;
    validate_address = false ;
    send_stdio = false ;
    var_data_staged = false;
    packets_copied = 0 ;
}

Trick::VariableServerSession::~VariableServerSession() {}

// Command to turn on log to varserver_log file
int Trick::VariableServerSession::set_log_on() {
    log = true;
    return(0) ;
}

// Command to turn off log to varserver_log file
int Trick::VariableServerSession::set_log_off() {
    log = false;
    return(0) ;
}

VS_COPY_MODE Trick::VariableServerSession::get_copy_mode() {
    return copy_mode;
}

VS_WRITE_MODE Trick::VariableServerSession::get_write_mode() {
    return write_mode;
}

void Trick::VariableServerSession::disconnect_references() {
    for (VariableReference * variable : session_variables) {
        variable->tagAsInvalid();
    }
}

long long Trick::VariableServerSession::get_next_tics() {
    if ( ! enabled ) {
        return TRICK_MAX_LONG_LONG ;
    }
    return next_tics ;
}

long long Trick::VariableServerSession::get_freeze_next_tics() {
    if ( ! enabled ) {
        return TRICK_MAX_LONG_LONG ;
    }
    return freeze_next_tics ;
}