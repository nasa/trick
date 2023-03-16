#include "trick/MulticastManager.hh"
#include <string.h>
#include <vector>

Trick::MulticastManager::MulticastManager() {
}

Trick::MulticastManager::~MulticastManager() {
    
}

int Trick::MulticastManager::restart () {
    // Keep address list the same, but we may need to get a new socket
    return initialize();
}


int Trick::MulticastManager::broadcast (std::string message) {
    if (!is_initialized()) {
        initialize();
    }
    const char * message_send = message.c_str();
    for (struct sockaddr_in& address : addresses) {
        sendto(mcast_socket , message_send , strlen(message_send) , 0 , (struct sockaddr *)&address , (socklen_t)sizeof(address)) ;
    }
    return 0;
}

int Trick::MulticastManager::addAddress (std::string addr, int port) {

    struct sockaddr_in mcast_addr;
    memset(&mcast_addr, 0, sizeof(mcast_addr));
    mcast_addr.sin_family = AF_INET;
    mcast_addr.sin_addr.s_addr = inet_addr(addr.c_str());
    mcast_addr.sin_port = htons((uint16_t) port);
    addresses.emplace_back(mcast_addr);
    return 0;
}

int Trick::MulticastManager::is_initialized () {
    return initialized;
}

int Trick::MulticastManager::initialize() {
    
    mcast_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (mcast_socket < 0) {
        perror("vs_mcast_init socket");
        return -1;
    }

    int option_value = 1;
    if (setsockopt(mcast_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &option_value, (socklen_t) sizeof(option_value)) < 0) {
        perror("setsockopt: reuseaddr");
        return -1;
    }
#ifdef SO_REUSEPORT
    if (setsockopt(mcast_socket, SOL_SOCKET, SO_REUSEPORT, (char *) &option_value, sizeof(option_value)) < 0) {
        perror("setsockopt: reuseport");
        return -1;
    }
#endif
    initialized = 1;
    return 0;
}