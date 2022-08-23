#include "trick/VariableServerSession.hh"
#include "trick/TrickConstant.hh"
#include "trick/exec_proto.h"
#include "trick/message_proto.h"
#include "trick/input_processor_proto.h"
#include "trick/realtimesync_proto.h"


Trick::VariableServerSession::VariableServerSession(ClientConnection * conn) {
    debug = 0;
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
    if (cycle_tics == 0) {
        cycle_tics = 1;
    }

    next_tics = TRICK_MAX_LONG_LONG ;
    freeze_next_tics = TRICK_MAX_LONG_LONG ;
    // multicast = false;
    connection = conn;
    byteswap = false ;
    validate_address = false ;
    send_stdio = false ;

    binary_data = false;
    byteswap = false;
    binary_data_nonames = false;
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

long long Trick::VariableServerSession::get_next_tics() const {
    if ( ! enabled ) {
        return TRICK_MAX_LONG_LONG ;
    }
    return next_tics ;
}

long long Trick::VariableServerSession::get_freeze_next_tics() const {
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

Trick::VariableReference * Trick::VariableServerSession::find_session_variable(std::string name) const {
    for (VariableReference * ref : session_variables) {
        // Look for matching name
        if (name.compare(ref->getName()) == 0) {
            return ref;
        }
    }

    return NULL;
}

double Trick::VariableServerSession::get_update_rate() const {
    return update_rate;
}

VS_WRITE_MODE Trick::VariableServerSession::get_write_mode () const {
    return write_mode;
}

VS_COPY_MODE Trick::VariableServerSession::get_copy_mode () const {
    return copy_mode;
}

std::ostream& Trick::operator<< (std::ostream& s, const Trick::VariableServerSession& session) {
    if (session.binary_data) {
        s << "    \"format\":\"BINARY\",\n";
    } else {
        s << "    \"format\":\"ASCII\",\n";
    }
    s << "    \"update_rate\":" << session.get_update_rate() << ",\n";

    s << "    \"variables\":[\n";

    int n_vars = (int)session.session_variables.size();
    for (int i=0 ; i<n_vars ; i++) {
        s << *(session.session_variables[i]);
        if ((n_vars-i)>1) {
            s << "," ;
        }
        s << "\n";
    }
    s << "    ]\n";

    return s;
}