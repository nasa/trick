#include "trick/VariableServerSession.hh"
#include "trick/TrickConstant.hh"
#include "trick/exec_proto.h"
#include "trick/message_proto.h"
#include "trick/input_processor_proto.h"


Trick::VariableServerSession::VariableServerSession(TCDevice * conn) {
    std::cout << "Creating Session" << std::endl;

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

    exit_cmd = false;

    pthread_mutex_init(&copy_mutex, NULL);
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

void Trick::VariableServerSession::sendMessage() {

}

int Trick::VariableServerSession::handleMessage() {
    // std::cout << "Handling message" << std::endl;

    char * incoming_msg = (char *) calloc(MAX_CMD_LEN, 1);
    int nbytes = recvfrom( connection->socket, incoming_msg, MAX_CMD_LEN, MSG_PEEK, NULL, NULL ) ;
    if (nbytes == 0 ) {
        return 0;
    }

    if (nbytes != -1) { // -1 means socket is nonblocking and no data to read
        /* find the last newline that is present on the socket */
        incoming_msg[nbytes] = '\0' ;
        char *last_newline = rindex( incoming_msg , '\n') ;

        /* if there is a newline then there is a complete command on the socket */
        if ( last_newline != NULL ) {
            /* only remove up to (and including) the last newline on the socket */
            int size = last_newline - incoming_msg + 1;
            // if ( conn_type == UDP ) {
            //     // Save the remote host information, that is where we are going to send replies.
            //     sock_size = sizeof(connection.remoteServAddr) ;
            //     nbytes = recvfrom( connection.socket, incoming_msg, size, 0 ,
            //         (struct sockaddr *)&connection.remoteServAddr, &sock_size ) ;
            // } else if ( conn_type == MCAST ) {
            //     // Save the remove host information for test against ourself.
            //     struct sockaddr_in s_in ;
            //     sock_size = sizeof(s_in) ;
            //     nbytes = recvfrom( connection.socket, incoming_msg, size, 0 ,
            //         (struct sockaddr *)&s_in, &sock_size ) ;
            //     // If this message is from us, then ignore it.
            //     if ( s_in.sin_addr.s_addr == self_s_in.sin_addr.s_addr and s_in.sin_port == self_s_in.sin_port) {
            //         nbytes = 0 ;
            //     }
            // } else {
                // We know where we are sending information, no need to save it.
                nbytes = recvfrom( connection->socket, incoming_msg, size, 0 , NULL, NULL ) ;
            // }
        } else {
            nbytes = 0 ;
        }
    }


    if ( nbytes > 0 ) {
        std::cout << "Client <" << connection->client_tag << "> sent message: " << incoming_msg << std::endl;

        int msg_len = nbytes ;
        if (debug >= 3) {
            message_publish(MSG_DEBUG, "%p tag=<%s> var_server received bytes = msg_len = %d\n", connection, connection->client_tag, nbytes);
        }

        incoming_msg[msg_len] = '\0' ;

        // if (vs->get_info_msg() || (debug >= 1)) {
        //     message_publish(MSG_DEBUG, "%p tag=<%s> var_server received: %s", connection, connection->client_tag, incoming_msg) ;
        // }
        // if (log) {
        //     message_publish(MSG_PLAYBACK, "tag=<%s> time=%f %s", connection->client_tag, exec_get_sim_time(), incoming_msg) ;
        // }

        char * stripped_msg = (char *) calloc (MAX_CMD_LEN, 1);
        for( int ii = 0 , jj = 0 ; ii <= msg_len ; ii++ ) {
            if ( incoming_msg[ii] != '\r' ) {
                stripped_msg[jj++] = incoming_msg[ii] ;
            }
        }

        // std::cout << "About to try to parse message" << std::endl;
        ip_parse(stripped_msg); /* returns 0 if no parsing error */
        // std::cout << "Finished parsing and executing message" << std::endl;

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