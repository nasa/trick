#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>

class Socket {

  public:
    Socket (std::string hostname, int port);

    int send (std::string message);
    int operator<< (std::string message);

    std::string receive ();
    void operator>> (std::string& ret);

  private:
    int port;
    std::string hostname;
    int socket_fd;

};
