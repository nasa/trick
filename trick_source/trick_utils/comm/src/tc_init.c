
/*
 * Set up a device on which to listen for connections
 */
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#ifndef __WIN32__
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#endif

#include "trick/tc.h"
#include "trick/tc_proto.h"
#include "trick/trick_byteswap.h"


int tc_init(TCDevice * listen_device) {
    return tc_init_with_connection_info( listen_device, TRICKCOMM_SOCKET_FAMILY, SOCK_STREAM, NULL, listen_device->port ) ;
}


int tc_init_with_connection_info( TCDevice * listen_device,
                                  int domain,
                                  int type,
                                  const char * address,
                                  unsigned short in_port ) {

    int listen_socket;
    int on = 1;
    //struct sockaddr_in s_in;
    struct hostent *ip_host ;

#ifdef __WIN32__
    WSADATA wsaData;
#endif
    struct sockaddr_in s_in;

    memset(&s_in, 0, sizeof(struct sockaddr_in)) ;

    if (!listen_device) {
        TrickErrorHndlr *temp_error_hndlr = NULL;
        trick_error_report(temp_error_hndlr, TRICK_ERROR_ALERT, __FILE__, __LINE__, "Listen device is null.");
        return (-1);
    }
#ifdef __WIN32__

    /* Initiate use of the Windows socket DLL */
    if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR) {
        trick_error_report(listen_device->error_handler,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__,
                           "tc_init: WSAStartup failed with error %d\n", WSAGetLastError());
        MessageBox(NULL, "Error", "tc_init", MB_OK);
        WSACleanup();
        return (TC_COULD_NOT_ACCEPT);
    }
#endif

    /* Open a TCP socket on which to listen for connections */
    listen_socket = socket(domain, type, TRICKCOMM_SOCKET_PROTO);
    if (listen_socket == TRICKCOMM_INVALID_SOCKET) {
        trick_error_report(listen_device->error_handler,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__,
                           "could not open socket on port %d: %s\n", listen_device->port, strerror(errno));
        return (TC_COULD_NOT_OPEN_SOCKET);
    }

    /* Do not replicate the socket's handle if this process is forked. */
    fcntl(listen_socket, F_SETFD, FD_CLOEXEC);

    /* Allow socket's bound address to be used by others */
    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, (const char *) &on, (socklen_t) sizeof(on));

    /* Turn off data buffering. This causes data to be written immediately to the receiving socket rather than queing
       it up until the transmit buffer is filled. */
    setsockopt(listen_socket, IPPROTO_TCP, TCP_NODELAY, (const char *) &on, (socklen_t) sizeof(on));

    memset(&s_in, 0 , sizeof(struct sockaddr_in)) ;
    s_in.sin_family = domain;
    char name[80];
    gethostname(name, (size_t) 80);
    if ( address == NULL || address[0] == '\0' ) {
        s_in.sin_addr.s_addr = INADDR_ANY ;
        listen_device->hostname = strdup(name) ;
    } else if ( ! strcmp( address, name )) {
        /* if name is the same as the default hostname, set s_addr to INADDR_ANY */
        s_in.sin_addr.s_addr = INADDR_ANY ;
    } else if ( inet_pton(AF_INET, address, (struct in_addr *)&s_in.sin_addr.s_addr) == 1 ) {
        /* some name other than the default name was given */
        listen_device->hostname = strdup(address) ;
    } else if ( (ip_host = gethostbyname(address)) != NULL ) {
        /* numeric character string address */
        memcpy((void *) &(s_in.sin_addr.s_addr), (const void *) ip_host->h_addr, (size_t) ip_host->h_length);
        listen_device->hostname = strdup(address) ;
    } else {
        listen_device->hostname = NULL ;
        fprintf(stderr, "cannot determine source address!\n") ;
        return -1 ;
    }
    listen_device->socket_type = type ;
    listen_device->port = in_port ;
    s_in.sin_port = htons((short) listen_device->port);

    /* Bind the socket */
    if (bind(listen_socket, (struct sockaddr *)&s_in, sizeof(s_in)) < 0) {
        trick_error_report(listen_device->error_handler,
                           TRICK_ERROR_ADVISORY, __FILE__, __LINE__,
                           "could not bind socket for port %d: %s\n", listen_device->port, strerror(errno));
        return (TC_COULD_NOT_BIND_SOCKET);
    }

    /* Get the actual port used if the incoming port was 0 */
    if ( listen_device->port == 0 ) {
        socklen_t s_in_size =  sizeof(s_in) ;
        getsockname( listen_socket , (struct sockaddr *)&s_in, &s_in_size) ;
        listen_device->port = ntohs(s_in.sin_port);
    }

    if ( type == SOCK_STREAM ) {
        /* Begin listening for connections */
        if (listen(listen_socket, SOMAXCONN) < 0) {
            /* listen can get EADDRINUSE when server & client are same machine */
            trick_error_report(listen_device->error_handler,
                               TRICK_ERROR_ADVISORY, __FILE__, __LINE__,
                               "could not listen on port %d: %s\n", listen_device->port, strerror(errno));
            return (TC_COULD_NOT_LISTEN_SOCKET);
        }
    }

    /* Set the fields in the user provided listen device. */
    listen_device->socket = listen_socket;
    listen_device->blockio_type = TC_COMM_BLOCKIO;
    listen_device->client_id = 0;
    strcpy(listen_device->client_tag, "<empty>");

    /* Set the byte info values (byte order and type sizes) in the listen device */
    TRICK_GET_BYTE_ORDER(listen_device->byte_info[TC_BYTE_ORDER_NDX]);
    listen_device->byte_info[TC_LONG_SIZE_NDX] = (unsigned char) sizeof(long);

    return (TC_SUCCESS);
}
