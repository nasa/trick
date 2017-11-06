
/*
 * Accept a connection from a communications client
 */

#ifndef __WIN32__
#include <errno.h>
#include <netinet/tcp.h>
#include <unistd.h>
#endif

#include "trick/tc.h"
#include "trick/tc_proto.h"
#include "trick/trick_byteswap.h"

#if _DMTCP
#include <dlfcn.h>
#include "dmtcpaware.h"
#endif


int tc_accept_(TCDevice * listen_device, TCDevice * device, const char *file, int line)
{
    socklen_t length;
    int size;
    int the_socket;
    int net_client_id;
    int on = 1;
    int tmp_rw;
    char *ptrL;
    char client_str[TC_TAG_LENGTH + 256];
    unsigned char byte_info[TC_BYTE_INFO_LENGTH];
    struct sockaddr_in s_in;

    memset(&s_in, 0, sizeof(struct sockaddr_in)) ;
    /* Accept On Listen Device */
    length = sizeof(s_in);
#if _DMTCP
    if( dmtcpIsEnabled() && device->dmtcp_use_real ) {
        const char real_accept_name[] = "_real_accept" ;
        void* dlhandle ;
        int (*real_accept_ptr)( int, struct sockaddr *,socklen_t *) = NULL ;

        dlhandle = dlopen( NULL, RTLD_LAZY) ;
        real_accept_ptr = (int (*)(int, struct sockaddr *,socklen_t *))dlsym( dlhandle , real_accept_name) ;
        if ( real_accept_ptr != NULL ) {
            printf("calling DMTCP _real_accept %s:%d\n", file , line) ;
            the_socket = (*real_accept_ptr)(listen_device->socket, (struct sockaddr *) &s_in, &length) ;
        } else {
            the_socket = accept(listen_device->socket, (struct sockaddr *) &s_in, &length);
        }
        dlclose(dlhandle) ;
    } else {
        the_socket = accept(listen_device->socket, (struct sockaddr *) &s_in, &length);
    }
#else
    the_socket = accept(listen_device->socket, (struct sockaddr *) &s_in, &length);
#endif

    sprintf(client_str, "(ID = %d  tag = %s)", listen_device->client_id, listen_device->client_tag);

    /* Check for error conditon on accept */
    if (the_socket == TRICKCOMM_INVALID_SOCKET) {
        trick_error_report(listen_device->error_handler,
                           TRICK_ERROR_ALERT, file, line,
                           "tc_accept: %s: error accepting the pending socket " "connection\n", client_str);
        return (TC_COULD_NOT_ACCEPT);
    }

    /* Turn off data buffering. This causes data to be sent immediately rather than queing it up until the transmit
       buffer is filled. */
    setsockopt(the_socket, IPPROTO_TCP, TCP_NODELAY, (const void *) &on, (socklen_t) sizeof(on));

    /* Check for error conditon on set socket option */
    if (the_socket < 0) {
        if (tc_errno == TRICKCOMM_EWOULDBLOCK) {
            trick_error_report(listen_device->error_handler,
                               TRICK_ERROR_ALERT, file,
                               line, "tc_accept: %s: error on set socket option: " "TC_EWOULDBLOCK", client_str);
            return (TC_EWOULDBLOCK);
        } else {
            trick_error_report(listen_device->error_handler,
                               TRICK_ERROR_ALERT, file,
                               line, "tc_accept: %s: error on set socket option: " "TC_COULD_NOT_ACCEPT", client_str);
            return (TC_COULD_NOT_ACCEPT);
        }
    }

    /*
     *  After accept, initialize device & Shake hands with client
     */

    /* Save the address of the new connection */
    device->client_addr = s_in.sin_addr;
    device->port = (int) s_in.sin_port;

    if (!device->disable_handshaking) {
        /* Read the client byte order */
        /* if -1, check for SIGNAL interupt and loop until data is returned */
        ptrL = (char *) device->byte_info;
        size = sizeof(device->byte_info);
        while ((tmp_rw = recv(the_socket, ptrL, (size_t) size, 0)) < 0 && tc_errno == TRICKCOMM_EINTR);
        if (tmp_rw < 0) {
            trick_error_report(listen_device->error_handler,
                               TRICK_ERROR_ALERT, file, line,
                               "tc_accept: %s: could not read byte info from client\n", client_str);
            CLOSE_SOCKET(the_socket);
            return (TC_COULD_NOT_ACCEPT);
        }

        /* Read the client id */
        /* if -1, check for SIGNAL interupt and loop until data is returned */
        ptrL = (char *) &net_client_id;
        size = sizeof(net_client_id);
        while ((tmp_rw = recv(the_socket, ptrL, (size_t) size, 0)) < 0 && tc_errno == TRICKCOMM_EINTR);
        if (tmp_rw < 0) {
            trick_error_report(listen_device->error_handler,
                               TRICK_ERROR_ALERT, file, line,
                               "tc_accept: %s: could not read client_id from client\n", client_str);
            CLOSE_SOCKET(the_socket);
            return (TC_COULD_NOT_ACCEPT);
        }

        /* Convert client ID from network to host byte order and save */
        device->client_id = ntohl((uint32_t) net_client_id);

        /* Read the client tag name If -1, check for SIGNAL interupt and loop until data is returned */
        ptrL = (char *) device->client_tag;
        size = sizeof(device->client_tag);
        while ((tmp_rw = recv(the_socket, ptrL, (size_t) size, 0)) < 0 && tc_errno == TRICKCOMM_EINTR);
        if (tmp_rw < 0) {
            trick_error_report(listen_device->error_handler,
                               TRICK_ERROR_ALERT, file, line,
                               "tc_accept: %s: could not read client_tag from client\n", client_str);
            CLOSE_SOCKET(the_socket);
            return (TC_COULD_NOT_ACCEPT);
        }

        /* Set the byte order and type size info of the server */
        TRICK_GET_BYTE_ORDER(byte_info[TC_BYTE_ORDER_NDX]);
        byte_info[TC_LONG_SIZE_NDX] = (unsigned char) sizeof(long);

        /* Send the server byte info.  It's a char array, so there's no need to convert to network byte order. If -1,
           check for SIGNAL interupt and loop until data is returned */
        ptrL = (char *) byte_info;
        size = sizeof(byte_info);
        while ((tmp_rw = send(the_socket, ptrL, (size_t) size, 0)) < 0 && tc_errno == TRICKCOMM_EINTR);
        if (tmp_rw < 0) {
            trick_error_report(listen_device->error_handler,
                               TRICK_ERROR_ALERT, file, line,
                               "tc_accept: %s: could not send client byte info to server\n", client_str);
            CLOSE_SOCKET(the_socket);
            return (TC_COULD_NOT_CONNECT);
        }
    }

    sprintf(client_str, "(ID = %d  tag = %s)", device->client_id, device->client_tag);

    trick_error_report(listen_device->error_handler, TRICK_ERROR_ADVISORY,
                       file, line, "tc_accept: %s: connected to client using port %d\n", client_str, device->port);

    /* Point this device to "the socket" received on accept to listen device */
    device->socket = the_socket;
    device->socket_type = SOCK_STREAM;

    /* Set the blocking flag for the connection */
    tc_blockio(device, device->blockio_type);

    return (TC_SUCCESS);
}
