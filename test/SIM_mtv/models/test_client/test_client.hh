#pragma once

#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <gtest/gtest.h>

#define MTV_TEST_PORT 40100
#define SOCKET_BUF 65536

// ---------------------------------------------------------------------------
// Minimal raw TCP socket (ASCII variable server protocol)
// ---------------------------------------------------------------------------
class Socket
{
public:
    Socket()
        : _fd(-1)
        , _ok(false)
    {
    }

    int init(const std::string& host, int port)
    {
        _fd = socket(AF_INET, SOCK_STREAM, 0);
        if (_fd < 0)
            return -1;

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port   = htons(port);
        if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0)
            return -1;

        if (connect(_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
            return -1;

        _ok = true;
        return 0;
    }

    void close()
    {
        ::close(_fd);
        _ok = false;
    }

    int send(const std::string& msg)
    {
        return ::send(_fd, msg.c_str(), msg.size(), 0);
    }

    // Read bytes until a '\n' is found or the buffer fills.
    // Returns the full line including the trailing newline.
    std::string recv_line(int timeout_sec = 5)
    {
        std::string line;
        char ch;
        fd_set fds;
        struct timeval tv;

        while (true)
        {
            FD_ZERO(&fds);
            FD_SET(_fd, &fds);
            tv.tv_sec  = timeout_sec;
            tv.tv_usec = 0;
            int ready  = select(_fd + 1, &fds, NULL, NULL, &tv);
            if (ready <= 0)
                break; // timeout or error
            int n = ::recv(_fd, &ch, 1, 0);
            if (n <= 0)
                break;
            line += ch;
            if (ch == '\n')
                break;
        }
        return line;
    }

    int operator<<(const std::string& msg) { return send(msg); }

    bool ok() const { return _ok; }

private:
    int _fd;
    bool _ok;
};

// ---------------------------------------------------------------------------
// GTest fixture – connects to the MTV sim's variable server
// ---------------------------------------------------------------------------
class MtvTest : public ::testing::Test
{
protected:
    MtvTest()
    {
        status = socket.init("localhost", MTV_TEST_PORT);
        if (status == 0)
        {
            socket << "trick.var_set_client_tag(\"MTV_GTest\")\n";
        }
    }
    ~MtvTest()
    {
        socket << "trick.var_exit()\n";
        socket.close();
    }

    Socket socket;
    int status;
};
