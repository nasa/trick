#include <unistd.h>
#include <iostream>
#include <sys/select.h>

#include "trick/ClientListener.hh"
#include "trick/tc_proto.h"



Trick::ClientListener::ClientListener () : _listen_dev(), initialized(false) {
    char hname[80];
    gethostname(hname , (size_t) 80 ) ;
    saved_source = std::string(hname);
    strcpy(_listen_dev.client_tag, "<empty>");
    tc_error(&_listen_dev, 0);
}

Trick::ClientListener::~ClientListener () {

}

int Trick::ClientListener::initialize(std::string hostname, int port) {
    int ret = tc_init_with_connection_info(&_listen_dev, AF_INET, SOCK_STREAM, hostname.c_str(), port);
    initialized = true;
    return ret;
}

int Trick::ClientListener::initialize() {
    int ret = tc_init(&_listen_dev);
    if (ret != TC_SUCCESS) {
        
        fprintf(stderr, "ERROR: Could not establish listen port for Variable Server. Aborting.\n");
        ret = -1 ;
    }
    initialized = true;
    return ret;
}


int Trick::ClientListener::setBlockMode(TCCommBlocking mode) {
    if (!initialized)
        return -1;

    return tc_blockio(&_listen_dev, mode);
}


bool Trick::ClientListener::checkForNewConnections() {
    if (!initialized)
        return -1;

    fd_set rfds;
    struct timeval timeout_time = { 2, 0 };
    FD_ZERO(&rfds);
    FD_SET(_listen_dev.socket, &rfds);
    timeout_time.tv_sec = 2 ;
    select(_listen_dev.socket + 1, &rfds, NULL, NULL, &timeout_time);

    return FD_ISSET(_listen_dev.socket, &rfds);
}


const char * Trick::ClientListener::getHostname () {
    if (!initialized)
        return "";

    return _listen_dev.hostname;
}


int Trick::ClientListener::getPort() {
    if (!initialized)
        return -1;

    return _listen_dev.port;
}


int Trick::ClientListener::disconnect() {
    if (!initialized)
        return -1;

    return tc_disconnect(&_listen_dev) ;    
}

bool Trick::ClientListener::validateSourceAddress(std::string requested_source_address) {
    char hname[80];
    static struct sockaddr_in s_in;
    gethostname(hname, (size_t) 80);

    // Test to see if the restart address is on this machine.  If it is not, it's not an error
    if ( strcmp( requested_source_address.c_str(), hname )) {
        if (! inet_pton(AF_INET, requested_source_address.c_str(), (struct in_addr *)&s_in.sin_addr.s_addr) ) {
            return false;
        }
    }

    return true;
}

int Trick::ClientListener::checkSocket() {
    if (!initialized)
        return -1;

    struct sockaddr_in s_in;
    int s_in_size =  sizeof(s_in) ;
    getsockname( _listen_dev.socket , (struct sockaddr *)&s_in, (socklen_t *)&s_in_size) ;
    printf("restart variable server message port = %d\n" , ntohs(s_in.sin_port)) ;
    _listen_dev.port = ntohs(s_in.sin_port);
}

bool Trick::ClientListener::isInitialized() {
    return initialized;
}

