#include "trick/MulticastGroup.hh"
#include <string.h>
#include <vector>



Trick::MulticastGroup::MulticastGroup() : MulticastGroup(new SystemInterface) {}

Trick::MulticastGroup::MulticastGroup (SystemInterface * system_interface) : _mcast_socket(0), _initialized(false), _system_interface(system_interface) {}

Trick::MulticastGroup::~MulticastGroup() {}

int Trick::MulticastGroup::restart () {
    // Keep address list the same, but we may need to get a new socket
    _system_interface = new SystemInterface();
    return initialize();
}


int Trick::MulticastGroup::broadcast (std::string message) {
    if (!isInitialized()) {
        return -1;
    }

    const char * message_send = message.c_str();
    for (struct sockaddr_in& address : _addresses) {
        int status = _system_interface->sendto(_mcast_socket , message_send , strlen(message_send) , 0 , (struct sockaddr *)&address , (socklen_t)sizeof(address)) ;
        if (status == -1) {
            char * readable_addr = inet_ntoa( address.sin_addr );
            std::string err_msg = "MulticastGroup: Failed to broadcast to address " + std::string(readable_addr);
            perror(err_msg.c_str());
        }
    }
    return 0;
}

int Trick::MulticastGroup::addAddress (std::string addr, int port) {
    auto in_addr = _system_interface->inet_addr(addr.c_str());
    if (in_addr == -1) {
        std::string error_msg = "MulticastGroup: Cannot add address " + addr + " to multicast group";
        perror(error_msg.c_str());
        return -1;
    }

    struct sockaddr_in mcast_addr;
    memset(&mcast_addr, 0, sizeof(mcast_addr));
    mcast_addr.sin_family = AF_INET;
    mcast_addr.sin_addr.s_addr = in_addr;
    mcast_addr.sin_port = htons((uint16_t) port);;
    _addresses.emplace_back(mcast_addr);
    return 0;
}

int Trick::MulticastGroup::isInitialized () {
    return _initialized;
}

int Trick::MulticastGroup::initialize() {
    
    _mcast_socket = _system_interface->socket(AF_INET, SOCK_DGRAM, 0);
    if (_mcast_socket < 0) {
        perror("MulticastGroup: socket");
        return -1;
    }

    int option_value = 1;
    if (_system_interface->setsockopt(_mcast_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &option_value, (socklen_t) sizeof(option_value)) < 0) {
        perror("MulticastGroup: setsockopt: reuseaddr");
        return -1;
    }
#ifdef SO_REUSEPORT
    if (_system_interface->setsockopt(_mcast_socket, SOL_SOCKET, SO_REUSEPORT, (char *) &option_value, sizeof(option_value)) < 0) {
        perror("MulticastGroup: setsockopt: reuseport");
        return -1;
    }
#endif
    _initialized = true;
    return 0;
}

int Trick::MulticastGroup::disconnect() {
    if (_initialized) {
        _system_interface->close(_mcast_socket);
    }
    return 0;
}