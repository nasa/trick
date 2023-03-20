#include "trick/VariableServerSession.hh"
#include "trick/TrickConstant.hh"
#include "trick/exec_proto.h"
#include "trick/message_proto.h"
#include "trick/input_processor_proto.h"
#include "trick/realtimesync_proto.h"


Trick::VariableServerSession::VariableServerSession() {
    _debug = 0;
    _enabled = true ;
    _log = false ;
    _copy_mode = VS_COPY_ASYNC ;
    _write_mode = VS_WRITE_ASYNC ;
    _frame_multiple = 1 ;
    _frame_offset = 0 ;
    _freeze_frame_multiple = 1 ;
    _freeze_frame_offset = 0 ;
    _update_rate = 0.1 ;
    _cycle_tics = (long long)(_update_rate * exec_get_time_tic_value()) ;
    if (_cycle_tics == 0) {
        _cycle_tics = 1;
    }

    _next_tics = TRICK_MAX_LONG_LONG ;
    _freeze_next_tics = TRICK_MAX_LONG_LONG ;
    _byteswap = false ;
    _validate_address = false ;
    _send_stdio = false ;

    _binary_data = false;
    _byteswap = false;
    _binary_data_nonames = false;

    _exit_cmd = false;
    _pause_cmd = false;

    pthread_mutex_init(&_copy_mutex, NULL);
}

Trick::VariableServerSession::~VariableServerSession() {
    for (unsigned int ii = 0 ; ii < _session_variables.size() ; ii++ ) {
        delete _session_variables[ii];
    }
 }


void Trick::VariableServerSession::set_connection(ClientConnection * conn) {
    _connection = conn;
}


// Command to turn on log to varserver_log file
int Trick::VariableServerSession::set_log_on() {
    _log = true;
    return(0) ;
}

// Command to turn off log to varserver_log file
int Trick::VariableServerSession::set_log_off() {
    _log = false;
    return(0) ;
}

bool Trick::VariableServerSession::get_pause() {
    return _pause_cmd ;
}

void Trick::VariableServerSession::set_pause( bool on_off) {
    _pause_cmd = on_off ;
}


bool Trick::VariableServerSession::get_exit_cmd() {
    return _exit_cmd ;
}

void Trick::VariableServerSession::pause_copy() {
    pthread_mutex_lock(&_copy_mutex);
}

void Trick::VariableServerSession::unpause_copy() {
    pthread_mutex_unlock(&_copy_mutex);
}

void Trick::VariableServerSession::disconnect_references() {
    for (VariableReference * variable : _session_variables) {
        variable->tagAsInvalid();
    }
}

long long Trick::VariableServerSession::get_next_tics() const {
    if ( ! _enabled ) {
        return TRICK_MAX_LONG_LONG ;
    }
    return _next_tics ;
}

long long Trick::VariableServerSession::get_freeze_next_tics() const {
    if ( ! _enabled ) {
        return TRICK_MAX_LONG_LONG ;
    }
    return _freeze_next_tics ;
}

int Trick::VariableServerSession::handle_message() {

    std::string received_message;
    int nbytes = _connection->read(received_message);
    if (nbytes > 0) {
        ip_parse(received_message.c_str()); /* returns 0 if no parsing error */
    }

    return nbytes;
}

Trick::VariableReference * Trick::VariableServerSession::find_session_variable(std::string name) const {
    for (VariableReference * ref : _session_variables) {
        // Look for matching name
        if (name.compare(ref->getName()) == 0) {
            return ref;
        }
    }

    return NULL;
}

double Trick::VariableServerSession::get_update_rate() const {
    return _update_rate;
}

VS_WRITE_MODE Trick::VariableServerSession::get_write_mode () const {
    return _write_mode;
}

VS_COPY_MODE Trick::VariableServerSession::get_copy_mode () const {
    return _copy_mode;
}

long long Trick::VariableServerSession::get_cycle_tics() const {
    return _cycle_tics;
}

int Trick::VariableServerSession::get_frame_multiple () const {
    return _frame_multiple;
}

int Trick::VariableServerSession::get_frame_offset () const {
    return _frame_offset;
}

int Trick::VariableServerSession::get_freeze_frame_multiple () const {
    return _freeze_frame_multiple;
}

int Trick::VariableServerSession::get_freeze_frame_offset () const {
    return _freeze_frame_offset;
}

bool Trick::VariableServerSession::get_enabled () const {
    return _enabled;
}

void Trick::VariableServerSession::set_freeze_next_tics(long long tics) {
    _freeze_next_tics = tics;
}

void Trick::VariableServerSession::set_next_tics(long long tics) {
    _next_tics = tics;
}

void Trick::VariableServerSession::set_exit_cmd() {
    _exit_cmd = true;
}



std::ostream& Trick::operator<< (std::ostream& s, const Trick::VariableServerSession& session) {
    if (session._binary_data) {
        s << "    \"format\":\"BINARY\",\n";
    } else {
        s << "    \"format\":\"ASCII\",\n";
    }
    s << "    \"update_rate\":" << session.get_update_rate() << ",\n";

    s << "    \"variables\":[\n";

    int n_vars = (int)session._session_variables.size();
    for (int i=0 ; i<n_vars ; i++) {
        s << *(session._session_variables[i]);
        if ((n_vars-i)>1) {
            s << "," ;
        }
        s << "\n";
    }
    s << "    ]\n";

    return s;
}