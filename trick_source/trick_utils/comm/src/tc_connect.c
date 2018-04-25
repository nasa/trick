
/*
 * Establish a connection with a communications server
 */

#include <string.h>

#ifndef __WIN32__
#include <netdb.h>
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


int tc_connect_(TCDevice * device, const char *file, int line)
{
    struct sockaddr_in sockin;
    struct hostent *ip_host;
    size_t size;
    int the_socket;
    int net_client_id;
    int on = 1;
    int tmp_rw;
    char *ptrL;
    char client_str[TC_TAG_LENGTH + 512];
    int ret;
#ifdef __WIN32__
    WSADATA wsaData;
#endif


    /*
     * Connection disabled?
     */
    if (!device) {
        TrickErrorHndlr *temp_error_hndlr = NULL;
        trick_error_report(temp_error_hndlr, TRICK_ERROR_ALERT, file, line, "Trying to connect a NULL device");
        return (-1);
    }

    if (device->disabled == TC_COMM_TRUE) {
        trick_error_report(device->error_handler,
                           TRICK_ERROR_ALERT, file, line, "Attempted to connect a disabled connection\n");
        return (TC_CONN_DISABLED);
    }

    if (device->hostname == NULL) {
        trick_error_report(device->error_handler, TRICK_ERROR_ALERT, file, line, "No hostname specified\n");
        return (TC_CONN_DISABLED);
    }

    /*
     * Set up error handling
     */
    if (device->client_tag[0] == '\0') {
        strcpy(device->client_tag, "<empty>");
    }
    sprintf(client_str, "(ID = %d tag = %s hostname = %s port = %d)",
            device->client_id, device->client_tag, device->hostname, device->port);

#ifdef __WIN32__

    /* Initiate use of the Windows socket DLL */
    if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR) {
        trick_error_report(device->error_handler,
                           TRICK_ERROR_ALERT, file, line,
                           "%s: WSAStartup failed with error %d\n", client_str, WSAGetLastError());
        MessageBox(NULL, "Error", "tc_connect", MB_OK);
        WSACleanup();
        return (TC_COULD_NOT_ACCEPT);
    }
#endif

   /*
    * Open socket
    */

    the_socket = socket( TRICKCOMM_SOCKET_FAMILY, SOCK_STREAM, TRICKCOMM_SOCKET_PROTO );
    if ( the_socket == TRICKCOMM_INVALID_SOCKET){

        trick_error_report(device->error_handler,TRICK_ERROR_ALERT, file, line, "%s: could not open socket: %s\n", client_str, strerror(errno));

        return (TC_COULD_NOT_OPEN_SOCKET);
    }


    /* Turn off data buffering. This causes data to be sent immediately rather than queuing it up until the transmit
       buffer is filled. */
    setsockopt(the_socket, IPPROTO_TCP, TCP_NODELAY, (const char *) &on, (socklen_t) sizeof(on));


    /*
     * Initialize socket address struct
     */
    memset((void *) &sockin, 0, (size_t) sizeof(sockin));
    sockin.sin_family = TRICKCOMM_SOCKET_FAMILY;
    sockin.sin_port = htons((uint16_t) device->port);

    ip_host = gethostbyname(device->hostname);

    if ( ip_host == NULL ){

        trick_error_report(device->error_handler, TRICK_ERROR_ALERT, file, line, "%s: host not found\n", client_str);
        CLOSE_SOCKET(the_socket);
        return (TC_HOST_NOT_FOUND);
    }
    memcpy((void *) &(sockin.sin_addr.s_addr), (const void *) ip_host->h_addr, (size_t) ip_host->h_length);


    /*
     *  Establish the connection to the selected server
     */
#if _DMTCP
    if( dmtcpIsEnabled() && device->dmtcp_use_real ) {
        const char real_connect_name[] = "_real_connect" ;
        void* dlhandle ;
        int (*real_connect_ptr)( int, struct sockaddr *,socklen_t) = NULL ;

        dlhandle = dlopen( NULL, RTLD_LAZY) ;
        real_connect_ptr = (int (*)(int, struct sockaddr *,socklen_t))dlsym( dlhandle , real_connect_name) ;
        if ( real_connect_ptr != NULL ) {
            printf("calling DMTCP _real_connect %s:%d\n", file , line) ;
            ret = (*real_connect_ptr)(the_socket, (struct sockaddr *) &sockin, (socklen_t) sizeof(sockin)) ;
        } else {
            ret = connect(the_socket, (struct sockaddr *) &sockin, (socklen_t) sizeof(sockin));
        }
        dlclose(dlhandle) ;
    } else {
        ret = connect(the_socket, (struct sockaddr *) &sockin, (socklen_t) sizeof(sockin));
    }
#else
    ret = connect(the_socket, (struct sockaddr *) &sockin, (socklen_t) sizeof(sockin));
#endif

    if ( ret < 0) {
        trick_error_report(device->error_handler,TRICK_ERROR_ALERT, file, line, "%s: could not connect to host: %s\n", client_str, strerror(errno));
        CLOSE_SOCKET(the_socket);
        return (TC_COULD_NOT_CONNECT);
    }


    /*
     * Shake hands with server (fill device struct)
     */

    if (!device->disable_handshaking) {

        /* Set the byte order and type size info of the client */
        TRICK_GET_BYTE_ORDER(device->byte_info[TC_BYTE_ORDER_NDX]);
        device->byte_info[TC_LONG_SIZE_NDX] = (unsigned char) sizeof(long);

        /* Send the client byte info.  It's a char array, so there's no need to convert to network byte order. If -1,
           check for SIGNAL interupt and loop until data is returned */
        ptrL = (char *) device->byte_info;
        size = sizeof(device->byte_info);
        while ((tmp_rw = send(the_socket, ptrL, size, 0)) < 0 && tc_errno == TRICKCOMM_EINTR);
        if (tmp_rw < 0) {
            trick_error_report(device->error_handler,
                               TRICK_ERROR_ALERT, file, line,
                               "%s: could not send client byte info to server\n", client_str);
            CLOSE_SOCKET(the_socket);
            return (TC_COULD_NOT_CONNECT);
        }

        /* Send client ID after converting from host to network byte order */
        net_client_id = htonl((uint32_t) device->client_id);

        /* If -1, check for signal interupt and loop until data is returned */
        ptrL = (char *) &net_client_id;
        size = sizeof(net_client_id);
        while ((tmp_rw = send(the_socket, ptrL, size, 0)) < 0 && tc_errno == TRICKCOMM_EINTR);
        if (tmp_rw < 0) {
            trick_error_report(device->error_handler,
                               TRICK_ERROR_ALERT, file, line, "%s: could not send client id to server\n", client_str);
            CLOSE_SOCKET(the_socket);
            return (TC_COULD_NOT_CONNECT);
        }

        /* Send the client tag name across */
        /* if -1, check for SIGNAL interupt and loop until data is returned */
        ptrL = device->client_tag;
        size = sizeof(device->client_tag);
        while ((tmp_rw = send(the_socket, ptrL, size, 0)) < 0 && tc_errno == TRICKCOMM_EINTR);
        if (tmp_rw < 0) {
            trick_error_report(device->error_handler,
                               TRICK_ERROR_ALERT, file, line, "%s: could not send client tag to server\n", client_str);
            CLOSE_SOCKET(the_socket);
            return (TC_COULD_NOT_CONNECT);
        }

        /* Read the server byte order */
        /* if -1, check for SIGNAL interupt and loop until data is returned */
        ptrL = (char *) device->byte_info;
        size = sizeof(device->byte_info);
        while ((tmp_rw = recv(the_socket, ptrL, size, 0)) < 0 && tc_errno == TRICKCOMM_EINTR);
        if (tmp_rw < 0) {
            trick_error_report(device->error_handler,
                               TRICK_ERROR_ALERT, file, line, "%s: could not read byte info from client\n", client_str);
            CLOSE_SOCKET(the_socket);
            return (TC_COULD_NOT_ACCEPT);
        }

        trick_error_report(device->error_handler,
                           TRICK_ERROR_ADVISORY, file, line,
                           "tc_connect: %s: successful connect to server on port %d\n", client_str, device->port);
    }

    /* Set device's socket descripter */
    device->socket = the_socket;
    device->socket_type = SOCK_STREAM ;

    /* Set the blocking flag for the connection */
    tc_blockio(device, device->blockio_type);

    return (TC_SUCCESS);

}
