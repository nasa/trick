/*
    PURPOSE: (A wrapper for system calls to facilitate testing.)
*/

#ifndef __SYSTEM_INTERFACE__
#define __SYSTEM_INTERFACE__

#include <arpa/inet.h>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <netdb.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class SystemInterface
{
public:
    virtual int socket(int domain, int type, int protocol)
    {
        return ::socket(domain, type, protocol);
    }

    virtual int setsockopt(int socket, int level, int option_name, const void * option_value, socklen_t option_len)
    {
        return ::setsockopt(socket, level, option_name, option_value, option_len);
    }

    virtual int bind(int socket, const struct sockaddr * address, socklen_t address_len)
    {
        return ::bind(socket, address, address_len);
    }

    virtual int getsockname(int socket, struct sockaddr * address, socklen_t * address_len)
    {
        return ::getsockname(socket, address, address_len);
    }

    virtual int listen(int socket, int backlog)
    {
        return ::listen(socket, backlog);
    }

    virtual int select(int nfds, fd_set * readfds, fd_set * writefds, fd_set * errorfds, struct timeval * timeout)
    {
        return ::select(nfds, readfds, writefds, errorfds, timeout);
    }

    virtual int close(int fildes)
    {
        return ::close(fildes);
    }

    virtual int getaddrinfo(const char * hostname,
                            const char * servname,
                            const struct addrinfo * hints,
                            struct addrinfo ** res)
    {
        return ::getaddrinfo(hostname, servname, hints, res);
    }

    virtual int fcntl(int fildes, int cmd, int arg)
    {
        return ::fcntl(fildes, cmd, arg);
    }

    virtual int shutdown(int socket, int how)
    {
        return ::shutdown(socket, how);
    }

    virtual int accept(int socket, struct sockaddr * address, socklen_t * address_len)
    {
        return ::accept(socket, address, address_len);
    }

    virtual ssize_t send(int socket, const void * buffer, size_t length, int flags)
    {
        return ::send(socket, buffer, length, flags);
    }

    virtual ssize_t sendto(int socket,
                           const void * buffer,
                           size_t length,
                           int flags,
                           const struct sockaddr * dest_addr,
                           socklen_t dest_len)
    {
        return ::sendto(socket, buffer, length, flags, dest_addr, dest_len);
    }

    virtual ssize_t recv(int socket, void * buffer, size_t length, int flags)
    {
        return ::recv(socket, buffer, length, flags);
    }

    virtual ssize_t recvfrom(
        int socket, void * buffer, size_t length, int flags, struct sockaddr * address, socklen_t * address_len)
    {
        return ::recvfrom(socket, buffer, length, flags, address, address_len);
    }

    virtual in_addr_t inet_addr(const char * cp)
    {
        return ::inet_addr(cp);
    }
};

#endif
