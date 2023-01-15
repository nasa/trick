#include "trick/VariableServerSession.hh"
#include "trick/TrickConstant.hh"
#include "trick/exec_proto.h"
#include "trick/message_proto.h"
#include "trick/input_processor_proto.h"
#include "trick/realtimesync_proto.h"


Trick::VariableServerSession::VariableServerSession(ClientConnection * conn) {
    debug = 3;
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

    exit_cmd = false;
    pause_cmd = false;


    // incoming_msg = (char *) calloc(MAX_CMD_LEN, 1);
    // stripped_msg = (char *) calloc(MAX_CMD_LEN, 1);


    pthread_mutex_init(&copy_mutex, NULL);
}

Trick::VariableServerSession::~VariableServerSession() {
    // free (incoming_msg);
    // free (stripped_msg);
}

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

int Trick::VariableServerSession::handleMessage() {

    std::string received_message = connection->read(ClientConnection::MAX_CMD_LEN);
    if (received_message.size() > 0) {
        ip_parse(received_message.c_str()); /* returns 0 if no parsing error */
    }
    
}

Trick::VariableReference * Trick::VariableServerSession::find_session_variable(std::string name) {
    for (VariableReference * ref : session_variables) {
        // Look for matching name
        if (name.compare(ref->getName()) == 0) {
            return ref;
        }
    }

    return NULL;
}

double Trick::VariableServerSession::get_update_rate() {
    return update_rate;
}

bool Trick::VariableServerSession::should_write_async() {
    return (write_mode == VS_WRITE_ASYNC) or
                 ((copy_mode == VS_COPY_ASYNC) and (write_mode == VS_WRITE_WHEN_COPIED)) or
                 (! is_real_time());
}

bool Trick::VariableServerSession::should_write_sync() {

}

bool Trick::VariableServerSession::should_copy_async() {
    return copy_mode == VS_COPY_ASYNC;
}

bool Trick::VariableServerSession::should_copy_sync() {

}
