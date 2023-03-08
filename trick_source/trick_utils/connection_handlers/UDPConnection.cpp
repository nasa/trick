#include "trick/UDPConnection.hh"
#include "trick/tc.h"
#include "trick/tc_proto.h"
#include <sstream>
#include <iostream>

Trick::UDPConnection::UDPConnection () : UDPConnection(new SystemInterface()) {}
Trick::UDPConnection::UDPConnection (SystemInterface * system_interface) : _started(false), _initialized(false), _port(0), _hostname(""), _system_interface(system_interface) {}

int Trick::UDPConnection::initialize(const std::string& hostname, int in_port) {
    std::string in_hostname = hostname;

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;

    if (in_hostname.size() == 0) {
        in_hostname = "localhost";
    }

    int err;
    if ((err = _system_interface->getaddrinfo(in_hostname.c_str(), std::to_string(in_port).c_str(), &hints, &res)) != 0) {
        std::cerr << "UDP socket: Unable to lookup address: " << gai_strerror(err) << std::endl;
        return -1;
    }

    if ((_socket = _system_interface->socket(res->ai_family, res->ai_socktype, 0)) < 0) {
        perror ("UDP socket: Unable to open socket");
        return -1;
    }

    int option_val = 1;

    // Do not replicate the socket's handle if this process is forked
    // Removing this will cause all kinds of problems when starting a variable server client via input file
    _system_interface->fcntl(_socket, F_SETFD, FD_CLOEXEC);

    // Allow socket's bound address to be used by others
    if (_system_interface->setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (const char *) &option_val, (socklen_t) sizeof(option_val)) != 0) {
        perror("UDP socket: Could not set socket to reuse addr");
        _system_interface->close (_socket);
        return -1;
    }

    // Bind to socket
    if (_system_interface->bind(_socket, res->ai_addr, res->ai_addrlen) < 0) {
        perror("UDP Socket: Could not bind to socket");
        _system_interface->close (_socket);
        return -1;
    } 

    // Check that correct port was bound to
    struct sockaddr_in s_in;
    socklen_t s_in_size =  sizeof(s_in) ;
    _system_interface->getsockname( _socket , (struct sockaddr *)&s_in, &s_in_size) ;
    int bound_port = ntohs(s_in.sin_port);

    if (in_port != 0 && bound_port != in_port) {
        std::cerr << "UDP socket: Could not bind to requested port " << in_port << std::endl;
        _system_interface->close(_socket);
        return -1;
    }

    // Save port number
    _port = bound_port;

    // Save printable hostname
    _hostname = inet_ntoa((struct in_addr)((struct sockaddr_in *) res->ai_addr)->sin_addr);

    setBlockMode(false);

    // Done!
    _initialized = true;
    return 0;
}

int Trick::UDPConnection::start() {
    // We've already set everything up, just make sure it was successful
    if (_initialized) {
        _started = true;
        return 0;
    }
    return -1;
}

int Trick::UDPConnection::write (char * message, int size) {
    if (!_started)
        return -1;

    socklen_t sock_size = sizeof(_remote_serv_addr);

    return _system_interface->sendto(_socket, message, size, 0, (struct sockaddr *) &_remote_serv_addr, sock_size );
}

int Trick::UDPConnection::write (const std::string& message) {
    if (!_started)
        return -1;

    char send_buf[message.length()+1];
    strcpy (send_buf, message.c_str());
    socklen_t sock_size = sizeof(_remote_serv_addr);

    return _system_interface->sendto(_socket, send_buf, message.length(), 0, (struct sockaddr *) &_remote_serv_addr, sock_size );
}

std::string Trick::UDPConnection::read  (int max_len) {
    if (!_started)
        return "";

    char incoming_msg[max_len];
    int nbytes = _system_interface->recvfrom( _socket, incoming_msg, MAX_CMD_LEN, MSG_PEEK, NULL, NULL ) ;
    if (nbytes == 0 ) {
        return 0;
    }

    if (nbytes != -1) { // -1 means socket is nonblocking and no data to read
        /* find the last newline that is present on the socket */
        incoming_msg[nbytes] = '\0' ;
        char *last_newline = rindex( incoming_msg , '\n') ;

        /* if there is a newline then there is a complete command on the socket */
        if ( last_newline != NULL ) {
            socklen_t sock_size = sizeof(_remote_serv_addr);
            /* Save the remote host information so we know where to send replies */
            /* only remove up to (and including) the last newline on the socket */
            int size = last_newline - incoming_msg + 1;
            nbytes = _system_interface->recvfrom( _socket, incoming_msg, size, 0 , (struct sockaddr *) &_remote_serv_addr, &sock_size ) ;
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

int Trick::UDPConnection::disconnect () {
    if (!_initialized) {
        return -1;
    }

    _system_interface->shutdown(_socket, SHUT_RDWR);
    _system_interface->close (_socket);
    _initialized = false;
    _started = false;

    return 0; 
}

int Trick::UDPConnection::setBlockMode(bool blocking) {
    int flag = _system_interface->fcntl(_socket, F_GETFL, 0);

    if (flag == -1) {
        std::string error_message = "Unable to get flags for fd " + std::to_string(_socket) + " block mode to " + std::to_string(blocking);
        perror (error_message.c_str());
        return -1;
    }

    if (blocking) {
        flag &= ~O_NONBLOCK;
    } else {
        flag |= O_NONBLOCK;
    }

    if (_system_interface->fcntl(_socket, F_SETFL, flag) == -1) {
        std::string error_message = "Unable to set fd " + std::to_string(_socket) + " block mode to " + std::to_string(blocking);
        perror (error_message.c_str());
        return -1;
    }

    return 0;
}

int Trick::UDPConnection::restart() {
    _system_interface = new SystemInterface();
}

bool Trick::UDPConnection::isInitialized() {
    return _started;
}

int Trick::UDPConnection::getPort() {
    return _port;
}

std::string Trick::UDPConnection::getHostname() {
    return _hostname;
}