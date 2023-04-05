#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <sstream>
#include <iostream>
#include <functional>
#include <cmath>
#include <ctype.h>
#include <pwd.h>
#include <fstream>

#include <gtest/gtest.h>


#define DOUBLE_TOL 1e-5
#define SOCKET_BUF_SIZE 204800

// Pretend that gtest was kind enough to provide an EXPECT_FEQ operator with a tolerance
#define EXPECT_FEQ(a,b) EXPECT_LE(fabs(a - b), DOUBLE_TOL)

class Socket {

  public:

    int max_retries = 10;

    Socket() : _initialized(false) {}
    int init(std::string hostname, int port, int mode = SOCK_STREAM) {
        _multicast_socket = false;

        _hostname = hostname;
        _port = port;
        int tries = 0;

        _socket_fd = socket(AF_INET, mode, 0);
        if (_socket_fd < 0) {
            std::cout << "Socket connection failed" << std::endl;
            return -1;
        }

        int value = 1;
        if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &value, (socklen_t) sizeof(value)) < 0) {
            std::cout << "init_multicast: Socket option failed" << std::endl;
            return -1;
        }

        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port); // convert to weird network byte format

        if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) { 
            std::cout << "Invalid address/ Address not supported" << std::endl; 
            return -1;
        } 

        tries = 0;
        int connection_status;

        connection_status = connect(_socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        if (connection_status < 0) {
            std::cout << "Connection failed" << std::endl;
            return -1;
        }

        _initialized = true;

        return 0;
    }
    #ifndef __APPLE__
    int init_multicast (std::string hostname, int port) {
        _multicast_socket = true;
        _hostname = hostname;
        _port = port;
        int tries = 0;

        _socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_socket_fd < 0) {
            std::cout << "init_multicast: Socket open failed" << std::endl;
            return -1;
        }

        int value = 1;
        if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &value, (socklen_t) sizeof(value)) < 0) {
            std::cout << "init_multicast: Socket option failed" << std::endl;
            return -1;
        }

        if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEPORT, (char *) &value, sizeof(value)) < 0) {
            perror("setsockopt: reuseport");
        }

        struct ip_mreq mreq;
        // Use setsockopt() to request that the kernel join a multicast group
        mreq.imr_multiaddr.s_addr = inet_addr(_hostname.c_str());
        mreq.imr_interface.s_addr = htonl(INADDR_ANY);
        if (setsockopt(_socket_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &mreq, (socklen_t) sizeof(mreq)) < 0) {
            std::cout << "init_multicast: setsockopt: ip_add_membership failed" << std::endl;
            return -1;
        }


        struct sockaddr_in sockin ;

        // Set up local interface
        // We must bind to the multicast address
        sockin.sin_family = AF_INET;
        sockin.sin_addr.s_addr = inet_addr(_hostname.c_str());
        sockin.sin_port = htons(_port);

        if ( bind(_socket_fd, (struct sockaddr *) &sockin, (socklen_t) sizeof(sockin)) < 0 ) {
            std::cout << "init_multicast: bind failed" << std::endl;
            return -1;
        }

        char loopch = 1;

        if(setsockopt(_socket_fd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch)) < 0)
        {
            perror("Setting IP_MULTICAST_LOOP error");
            return -1;
        }
        
        _initialized = true;
        return 0;
    }

    #endif

    int send (std::string message) {
        // weird syntax I've never used before - since the send syscall that i'm trying to use is overloaded in this class,
        // I have to append :: to the front of it so that the compiler knows to look in the global namespace
        int success = ::send(_socket_fd, message.c_str(), message.size(), 0);
        if (success < message.size()) {
            std::cout << "init_multicast: Failed to send message" << std::endl;
        }
        return success;
    }

    int operator<< (std::string message) {
        return send(message);
    }

    std::string receive () {
        char buffer[SOCKET_BUF_SIZE];
        int numBytes = recv(_socket_fd, buffer, SOCKET_BUF_SIZE, 0);
        if (numBytes < 0) {
        } else if (numBytes < SOCKET_BUF_SIZE) {
            buffer[numBytes] = '\0';
        }

        return std::string(buffer);
    }

    void operator>> (std::string& ret) {
        ret = receive();
    }

    void operator>> (std::ostream& stream) {
        stream << receive();
    }

    std::vector<unsigned char> receive_bytes() {
        unsigned char buffer[SOCKET_BUF_SIZE];
        int numBytes = recv(_socket_fd, buffer, SOCKET_BUF_SIZE, 0);
        if (numBytes < 0) {
            std::cout << "init_multicast: Failed to read from socket" << std::endl;
        } 

        std::vector<unsigned char> bytes;
        for (int i = 0; i < numBytes; i++) {
            bytes.push_back(buffer[i]);
        }

        return bytes;
    }

    bool check_for_message_availible(long timeout_sec = 2) {
        fd_set read_fd_set;
        struct timeval timeout = { .tv_sec = timeout_sec, .tv_usec = 0 };
        FD_ZERO(&read_fd_set);
        FD_SET(_socket_fd, &read_fd_set);

        // I have one question for the designers of the interface for this syscall: why
        select(_socket_fd+1, &read_fd_set, NULL, NULL, &timeout);

        return FD_ISSET(_socket_fd, &read_fd_set);
    }

    void clear_buffered_data() {
        // Poll the socket
        if (check_for_message_availible(0)) {
            // Receive into the void if there is a message
            receive();
        }
        // otherwise no worries
    }

    void close() {
        ::close(_socket_fd);
    }

  private:
    int _port;
    std::string _hostname;
    int _socket_fd;
    bool _initialized;
    bool _multicast_socket;

};

class VariableServerTest : public ::testing::Test {
    protected:
        VariableServerTest() {
            socket_status = socket.init("localhost", 40000);
            if (socket_status == 0) {
                std::stringstream request;
                request << "trick.var_set_client_tag(\"VSTest";
                request << numSession++;
                request << "\") \n";

                socket << request.str();
            }
        }
        ~VariableServerTest() {
            socket << "trick.var_exit()\n";
            socket.close();
        }

        Socket socket;
        int socket_status;
        
        static int numSession;
};