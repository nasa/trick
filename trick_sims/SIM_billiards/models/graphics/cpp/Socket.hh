#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class Socket
{
public:
    Socket();
    int init(std::string hostname, int port);

    int send(std::string message);
    int operator<<(std::string message);

    std::string receive();
    void operator>>(std::string & ret);

private:
    int _port;
    std::string _hostname;
    int _socket_fd;
    bool _initialized;
};
