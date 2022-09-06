#include "trick/MulticastManager.hh"
#include <string.h>

Trick::MulticastManager::MulticastManager() {
    mcast_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (mcast_socket < 0) {
        perror("vs_mcast_init socket");
        // Maybe should do this in an init thing
    }

    int option_value = 1;
    if (setsockopt(mcast_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &option_value, (socklen_t) sizeof(option_value)) < 0) {
        perror("setsockopt: reuseaddr");
    }
#ifdef SO_REUSEPORT
    if (setsockopt(mcast_socket, SOL_SOCKET, SO_REUSEPORT, (char *) &option_value, sizeof(option_value)) < 0) {
        perror("setsockopt: reuseport");
    }
#endif

}

Trick::MulticastManager::~MulticastManager() {

}

int Trick::MulticastManager::broadcast (std::string message) {
    const char * message_send = message.c_str();
    for (struct sockaddr_in& address : addresses) {
        sendto(mcast_socket , message_send , strlen(message_send) , 0 , (struct sockaddr *)&address , (socklen_t)sizeof(address)) ;
    }
}

int Trick::MulticastManager::addAddress (std::string addr, int port) {
    struct sockaddr_in mcast_addr;
    memset(&mcast_addr, 0, sizeof(mcast_addr));
    mcast_addr.sin_family = AF_INET;
    mcast_addr.sin_addr.s_addr = inet_addr(addr.c_str());
    mcast_addr.sin_port = htons((uint16_t) port);
    addresses.emplace_back(mcast_addr);
}