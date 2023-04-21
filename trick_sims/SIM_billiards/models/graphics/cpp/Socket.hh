#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>

class Socket {

  public:
    Socket ();
    int init(std::string hostname, int port);

    int send (std::string message);
    int operator<< (std::string message);

    std::string receive ();
    void operator>> (std::string& ret);

  private:
    int _port;
    std::string _hostname;
    int _socket_fd;
    bool _initialized;

};
