#include "trick/UDPConnection.hh"
#include "trick/tc.h"
#include "trick/tc_proto.h"
#include <sstream>
#include <iostream>

Trick::UDPConnection::UDPConnection () : _connected(false) {}


int Trick::UDPConnection::initialize() {
    _device.disabled = TC_COMM_FALSE ;
    _device.disable_handshaking = TC_COMM_TRUE ;
    _device.blockio_limit = 0.0 ;
    _device.blockio_type = TC_COMM_BLOCKIO ;
    _device.client_id = 0 ;
    strcpy(_device.client_tag, "") ;
    _device.error_handler = (TrickErrorHndlr *) calloc(1, (int)sizeof(TrickErrorHndlr));
    _device.error_handler->report_level = TRICK_ERROR_CAUTION;
}

int Trick::UDPConnection::establish_connection() {
    // We should have already established the socket, just return the status
    if (_connected)
        return 0;

    return -1;
}

int Trick::UDPConnection::initialize_udp(const std::string& hostname, int port) {
    initialize();

    if (tc_init_with_connection_info(&_device, AF_INET, SOCK_DGRAM, hostname.c_str(), port) != 0)
        return -1;
    
    set_block_mode (TC_COMM_NOBLOCKIO);

    _connected = true;
    return 0;
}

int Trick::UDPConnection::write (char * message, int size) {
    int ret = tc_write(&_device, message, size);
    return ret;
}

int Trick::UDPConnection::write (const std::string& message) {
    char send_buf[message.length()+1];
    strcpy (send_buf, message.c_str());
    int ret = tc_write(&_device, send_buf, message.length());
    return ret;
}

std::string Trick::UDPConnection::read  (int max_len) {
    char incoming_msg[max_len];
    int nbytes = recvfrom( _device.socket, incoming_msg, MAX_CMD_LEN, MSG_PEEK, NULL, NULL ) ;
    if (nbytes == 0 ) {
        return 0;
    }

    if (nbytes != -1) { // -1 means socket is nonblocking and no data to read
        /* find the last newline that is present on the socket */
        incoming_msg[nbytes] = '\0' ;
        char *last_newline = rindex( incoming_msg , '\n') ;

        /* if there is a newline then there is a complete command on the socket */
        if ( last_newline != NULL ) {
            socklen_t sock_size = sizeof(_device.remoteServAddr);
            /* Save the remote host information so we know where to send replies */
            /* only remove up to (and including) the last newline on the socket */
            int size = last_newline - incoming_msg + 1;
            nbytes = recvfrom( _device.socket, incoming_msg, size, 0 , (struct sockaddr *) &_device.remoteServAddr, &sock_size ) ;
        } else {
            nbytes = 0 ;
        }
    }

    std::stringstream msg_stream;

    if ( nbytes > 0 ) {

        int msg_len = nbytes ;
        incoming_msg[msg_len] = '\0' ;

        for( int ii = 0 , jj = 0 ; ii <= msg_len ; ii++ ) {
            if ( incoming_msg[ii] != '\r' ) {
                msg_stream << incoming_msg[ii] ;
            }
        }
    }

    return msg_stream.str();
}


std::string Trick::UDPConnection::get_client_tag () {
    return std::string(_device.client_tag);
}

int Trick::UDPConnection::set_client_tag(std::string tag) {
    // Max size of device client tag is 80
    
    // TODO: Make 80 a constant somewhere, probably in TC device
    if (tag.length() >= 80) {
        tag.resize(79);
    }
    strcpy(_device.client_tag, tag.c_str());
    return 0;
}

int Trick::UDPConnection::get_socket() {
    return _device.socket;
}

int Trick::UDPConnection::disconnect () {
    return tc_disconnect(&_device);
}

int Trick::UDPConnection::set_block_mode(int block_mode) {
    return tc_blockio(&_device, (TCCommBlocking)block_mode);
}

int Trick::UDPConnection::set_error_reporting (bool on) {
    return tc_error(&_device, (int)on);
}