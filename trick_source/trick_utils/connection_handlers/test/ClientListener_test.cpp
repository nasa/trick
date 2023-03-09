
#include <gtest/gtest.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <sys/select.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "trick/ClientListener.hh"
#include "SystemInterfaceMock/SystemInterfaceMock.hh"


class ClientListenerTest : public testing::Test {

   protected:
      ClientListenerTest() : system_context(new SystemInferfaceMock()), listener (system_context) {}
      ~ClientListenerTest(){}

      SystemInferfaceMock * system_context;
      Trick::ClientListener listener;
};


TEST_F( ClientListenerTest, initialized ) {
    EXPECT_EQ(listener.isInitialized(), false);
}

TEST_F( ClientListenerTest, initialize_localhost_0 ) {
    // ARRANGE
    // Look up the hostname
    char name[80];
    gethostname(name, (size_t) 80);

    // ACT
    listener.initialize("localhost", 0);

    // ASSERT
    EXPECT_EQ(listener.isInitialized(), true);
    EXPECT_EQ(listener.getHostname(), std::string(name));
}

TEST_F( ClientListenerTest, initialize_localhost_54321 ) {
    // ARRANGE
    // ACT
    listener.initialize("localhost", 54321);

    // ASSERT
    EXPECT_EQ(listener.isInitialized(), true);
    EXPECT_EQ(listener.getPort(), 54321);
}

TEST_F( ClientListenerTest, initialize_no_args ) {
    // ARRANGE
    // ACT
    listener.initialize();

    // ASSERT
    EXPECT_EQ(listener.isInitialized(), true);
    EXPECT_GT(listener.getPort(), 1000);
}

TEST_F( ClientListenerTest, initialize_localhost_numerical_54321 ) {
    // ARRANGE
    // ACT
    listener.initialize("127.0.0.1", 54321);

    // ASSERT
    EXPECT_EQ(listener.isInitialized(), true);
    EXPECT_EQ(listener.getPort(), 54321);
}

TEST_F( ClientListenerTest, initialize_invalid_hostname ) {
    // ARRANGE
    // ACT
    listener.initialize("some_invalid_hostname", 0);

    // ASSERT
    EXPECT_EQ(listener.isInitialized(), false);
}

TEST_F( ClientListenerTest, failed_socket ) {
    // ARRANGE
    system_context->register_socket_impl([](int a, int b, int c) { 
        errno = EPERM;
        return -1; 
    });

    // ACT
    listener.initialize("localhost", 54321);

    // ASSERT
    EXPECT_EQ(listener.isInitialized(), false);
}

TEST_F( ClientListenerTest, failed_setsockopt_reuseaddr ) {
    // ARRANGE
    system_context->register_setsockopt_impl([](int sockfd, int level, int optname, const void *optval, socklen_t optlen) { 
        errno = EINVAL;
        return -1; 
    });

    // ACT
    listener.initialize("localhost", 54321);

    // ASSERT
    EXPECT_EQ(listener.isInitialized(), false);
}

TEST_F( ClientListenerTest, failed_setsockopt_buffering ) {
    // ARRANGE
    system_context->register_setsockopt_impl([](int sockfd, int level, int optname, const void *optval, socklen_t optlen) { 
        if (level == IPPROTO_TCP && optname == TCP_NODELAY) {
            errno = ENOTSOCK;
            return -1; 
        }

        return 0;
    });

    // ACT
    listener.initialize("localhost", 54321);

    // ASSERT
    EXPECT_EQ(listener.isInitialized(), false);
}

TEST_F( ClientListenerTest, failed_bind ) {
    // ARRANGE
    system_context->register_bind_impl([](int sockfd, const struct sockaddr *addr,socklen_t addrlen) { 
        errno = EADDRINUSE;
        return -1;
    });

    // ACT
    listener.initialize("localhost", 54321);

    // ASSERT
    EXPECT_EQ(listener.isInitialized(), false);
}

TEST_F( ClientListenerTest, failed_sockname ) {
    // ARRANGE
    system_context->register_getsockname_impl([](int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
        ((struct sockaddr_in *) addr)->sin_port = htons(1234);
        return 0;
    });

    // ACT
    listener.initialize("localhost", 54321);

    // ASSERT
    EXPECT_EQ(listener.isInitialized(), false);
}

TEST_F( ClientListenerTest, failed_listen ) {
    // ARRANGE
    system_context->register_listen_impl([](int sockfd, int backlog) {
        errno = EADDRINUSE;
        return -1;
    });

    // ACT
    listener.initialize("localhost", 54321);

    // ASSERT
    EXPECT_EQ(listener.isInitialized(), false);
}

TEST_F( ClientListenerTest, checkForNewConnections_uninitialized ) {
    // ARRANGE
    // ACT
    bool result = listener.checkForNewConnections();

    // ASSERT
    EXPECT_EQ(result, false);
}

TEST_F( ClientListenerTest, checkForNewConnections ) {
    // ARRANGE
    int socket_fd;
    system_context->register_socket_impl([&socket_fd](int a, int b, int c) {
        socket_fd = ::socket(a, b, c);
        return socket_fd;
    });
    system_context->register_select_impl([&socket_fd](int nfds, fd_set *readfds, fd_set *writefds,fd_set *exceptfds, struct timeval *timeout) {
        FD_SET(socket_fd, readfds);
        return 1;
    });
    listener.initialize();

    // ACT
    bool result = listener.checkForNewConnections();
    
    // ASSERT
    EXPECT_EQ(result, true);
}

TEST_F( ClientListenerTest, checkForNewConnections_select_error ) {
    // ARRANGE
    system_context->register_select_impl([](int nfds, fd_set *readfds, fd_set *writefds,fd_set *exceptfds, struct timeval *timeout) {
        return -1;
    });
    listener.initialize();

    // ACT
    bool result = listener.checkForNewConnections();

    // ASSERT
    EXPECT_EQ(result, false);
}

TEST_F( ClientListenerTest, setBlockMode_false ) {
    // ARRANGE
    int socket_fd;
    system_context->register_socket_impl([&socket_fd](int a, int b, int c) {
        socket_fd = ::socket(a, b, c);
        return socket_fd;
    });
    listener.initialize();

    // ACT
    int status = listener.setBlockMode(false);

    // ASSERT
    EXPECT_EQ(status, 0);

    int flag = fcntl(socket_fd, F_GETFL, 0);
    EXPECT_TRUE(flag & O_NONBLOCK);
}

TEST_F( ClientListenerTest, setBlockMode_nonblocking) {
    // ARRANGE
    int socket_fd;
    system_context->register_socket_impl([&socket_fd](int a, int b, int c) {
        socket_fd = ::socket(a, b, c);
        return socket_fd;
    });
    listener.initialize();

    // ACT
    int status = listener.setBlockMode(false);

    // ASSERT
    EXPECT_EQ(status, 0);

    int flag = fcntl(socket_fd, F_GETFL, 0);
    EXPECT_TRUE(flag & O_NONBLOCK);
}


TEST_F( ClientListenerTest, setBlockMode_blocking) {
    // ARRANGE
    int socket_fd;
    system_context->register_socket_impl([&socket_fd](int a, int b, int c) {
        socket_fd = ::socket(a, b, c);
        return socket_fd;
    });
    listener.initialize();

    // ACT
    int status = listener.setBlockMode(true);

    // ASSERT
    EXPECT_EQ(status, 0);

    int flag = fcntl(socket_fd, F_GETFL, 0);
    EXPECT_FALSE(flag & O_NONBLOCK);
}

TEST_F( ClientListenerTest, setBlockMode_fcntl_getfl_fail) {
    // ARRANGE
    int socket_fd;
    system_context->register_socket_impl([&socket_fd](int a, int b, int c) {
        socket_fd = ::socket(a, b, c);
        return socket_fd;
    });
    system_context->register_fcntl_impl([](int a, int b, int c) {
        errno = EACCES;
        return -1;
    });
    listener.initialize();

    // ACT
    int status = listener.setBlockMode(true);

    // ASSERT
    EXPECT_EQ(status, -1);
}

TEST_F( ClientListenerTest, setBlockMode_fcntl_setfl_fail) {
    // ARRANGE
    int socket_fd;
    system_context->register_socket_impl([&socket_fd](int a, int b, int c) {
        socket_fd = ::socket(a, b, c);
        return socket_fd;
    });
    system_context->register_fcntl_impl([](int a, int cmd, int c) {
        if (cmd == F_SETFL) {
            errno = EBADF;
            return -1;
        }
        return 0;
    });

    listener.initialize();

    // ACT
    int status = listener.setBlockMode(true);

    // ASSERT
    EXPECT_EQ(status, -1);
}


TEST_F( ClientListenerTest, validateSourceAddress_localhost) {
    // ARRANGE
    // ACT
    bool status = listener.validateSourceAddress("localhost");

    // ASSERT
    EXPECT_EQ(status, true);
}

TEST_F( ClientListenerTest, validateSourceAddress_junk) {
    // ARRANGE
    // ACT
    bool status = listener.validateSourceAddress("alsdkfjgalkdj");

    // ASSERT
    EXPECT_EQ(status, false);
}

TEST_F( ClientListenerTest, checkSocket) {
    // ARRANGE
    listener.initialize();
    int port = listener.getPort();

    // ACT
    int status = listener.checkSocket();
    
    // ASSERT
    EXPECT_EQ(status, 0);
    EXPECT_EQ(listener.getPort(), port);
}

TEST_F( ClientListenerTest, checkSocket_uninitialized) {
    // ARRANGE
    // ACT
    int status = listener.checkSocket();
    
    // ASSERT
    EXPECT_EQ(status, -1);
}

TEST_F( ClientListenerTest, disconnect) {
    // ARRANGE
    listener.initialize();

    // ACT
    int status = listener.disconnect();
    
    // ASSERT
    EXPECT_EQ(status, 0);
}

TEST_F( ClientListenerTest, disconnect_uninitialized) {
    // ARRANGE
    // ACT
    int status = listener.disconnect();
    
    // ASSERT
    EXPECT_EQ(status, -1);
}

TEST_F( ClientListenerTest, setupNewConnection) {
    // ARRANGE
    listener.initialize();

    // ACT
    Trick::TCPConnection * connection = listener.setUpNewConnection();
    
    // ASSERT
    EXPECT_TRUE(connection != NULL);
}

TEST_F( ClientListenerTest, setupNewConnection_uninitialized) {
    // ARRANGE

    // ACT
    Trick::TCPConnection * connection = listener.setUpNewConnection();
    
    // ASSERT
    EXPECT_TRUE(connection == NULL);
}