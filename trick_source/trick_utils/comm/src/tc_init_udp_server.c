/*
   PURPOSE: (Set up device for UDP communication as a server.)

   REFERENCE: ((None))

   ASSUMPTIONS AND LIMITATIONS:
   ((udp_server_device->port must be specified.) (udp_server_device->blockio_type should be specified; defaults to
   blocking.))

   CLASS: (scheduled)

   LIBRARY DEPENDENCY: ((tc_init_udp_server.o))

   PROGRAMMERS: (((M. Will) (L3-Com) (31 October 2006) (--) (CAIL))) */

#ifndef __WIN32__
#include <netinet/tcp.h>
#endif

#include "trick/tc.h"
#include "trick/tc_proto.h"
#include "trick/trick_byteswap.h"

int tc_init_udp_server(         /* RETURN: -- 0 for success */
                          TCDevice * udp_server_device)
{                                      /* INOUT: -- Device structure for the UDP server */

    struct sockaddr_in sockin;

    unsigned int yes = 1;

    if (!udp_server_device) {
        trick_error_report(udp_server_device->error_handler,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__, "UDP device is null.");
        return (-1);
    }

    /* Open a UDP socket */
    udp_server_device->socket = socket(AF_INET, SOCK_DGRAM, 0);
    udp_server_device->socket_type = SOCK_DGRAM ;

    if (udp_server_device->socket == TRICKCOMM_INVALID_SOCKET) {
        perror("tc_init_udp_server: socket call failed");

        trick_error_report(udp_server_device->error_handler,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__, "Error creating UDP socket.");

        return (TRICKCOMM_INVALID_SOCKET);
    }

    /* Allow socket's bound address to be used by others */
    if (setsockopt(udp_server_device->socket, SOL_SOCKET, SO_REUSEADDR, &yes, (socklen_t) sizeof(yes)) < 0) {
        perror("tc_init_udp_server.c: SO_REUSEADDR error");

        trick_error_report(udp_server_device->error_handler,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__,
                           "Could not allow multiple sockets to use the same port = \n", (udp_server_device->port));

        return (-1);
    }


    /* Turn off data buffering. This causes data to be written immediately to the receiving socket rather than queing
       it up until the transmit buffer is filled. */
    setsockopt(udp_server_device->socket, IPPROTO_TCP, TCP_NODELAY, (const char *) &yes, (socklen_t) sizeof(yes));


    sockin.sin_addr.s_addr = INADDR_ANY;
    sockin.sin_family = TRICKCOMM_SOCKET_FAMILY;
    sockin.sin_port = htons((short) (udp_server_device->port));

    if (bind(udp_server_device->socket, (struct sockaddr *) &sockin, (socklen_t) sizeof(sockin)) < 0) {
        perror("tc_init_udp_server: bind error");

        trick_error_report(udp_server_device->error_handler,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__,
                           "Could not bind UDP socket for port %d\n", (udp_server_device->port));

        return (TC_COULD_NOT_BIND_SOCKET);
    }

    udp_server_device->client_id = 0;
    strcpy(udp_server_device->client_tag, "<empty>");

    /* Set the byte info values (byte order and type sizes) */
    TRICK_GET_BYTE_ORDER(udp_server_device->byte_info[TC_BYTE_ORDER_NDX]);
    udp_server_device->byte_info[TC_LONG_SIZE_NDX] = (unsigned char) sizeof(long);

    return (TC_SUCCESS);

}
