/*
   PURPOSE: (Set up device for UDP communication as a client.)

   REFERENCE: ((None))

   ASSUMPTIONS AND LIMITATIONS: ((udp_client_device->port is required as input.)
    (udp_client_device->hostname is required as input.)
    (udp_client_device->blockio_type defaults to blocking.))

   CLASS: (scheduled)

   LIBRARY DEPENDENCY: ((tc_init_udp_client.o) (tc_blockio.o))

   PROGRAMMERS: (((M. Will) (L3-Com) (31 October 2006) (--) (CAIL))) */

#ifndef __WIN32__
#include <netdb.h>
#include <netinet/tcp.h>
#include <unistd.h>
#endif

#include "trick/tc.h"
#include "trick/tc_proto.h"
#include "trick/trick_byteswap.h"


int tc_init_udp_client(TCDevice * udp_client_device)
{

    int debug = 1;

    struct hostent *h;
    int the_socket;
    struct sockaddr_in sockin;

    memset(&sockin, 0 , sizeof(struct sockaddr_in)) ;

    if (!udp_client_device) {
        trick_error_report(NULL,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__, "UDP device is null.");
        return (-1);
    }

    if (udp_client_device->disabled == TC_COMM_TRUE) {
        trick_error_report(udp_client_device->error_handler,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__, "Attempted to connect a disabled connection\n");
        return (TC_CONN_DISABLED);
    }

    /* Open a UDP socket */
    the_socket = socket(AF_INET, SOCK_DGRAM, 0);

    if (the_socket == TRICKCOMM_INVALID_SOCKET) {
        perror("tc_init_udp_client: socket creation error");

        trick_error_report(udp_client_device->error_handler,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__, "Error creating UDP socket.");

        return (TC_COULD_NOT_OPEN_SOCKET);
    }

    sockin.sin_family = TRICKCOMM_SOCKET_FAMILY;
    sockin.sin_addr.s_addr = htonl(INADDR_ANY);
    sockin.sin_port = htons(0);

    if (bind(the_socket, (struct sockaddr *) &sockin, (socklen_t) sizeof(sockin)) < 0) {
        perror("tc_init_udp_client: bind error");

        trick_error_report(udp_client_device->error_handler,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__,
                           "Could not bind socket for port %d\n", (udp_client_device->port));

        return (TC_COULD_NOT_BIND_SOCKET);
    }

    h = gethostbyname(udp_client_device->hostname);

    if (h == NULL) {
        trick_error_report(udp_client_device->error_handler,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__, "Host %s not found.\n", udp_client_device->hostname);

        return (TC_HOST_NOT_FOUND);
    }

    if (debug) {
        char this_host[64];
        char addr_list[129];    // IPV6 addresses can have 128 bits + 1 for null character
        if (gethostname(this_host, sizeof(this_host)) < 0) {
            perror("gethostname");
            this_host[0] = 0;
        }
        // For some unknown reason, this causes a seg fault only on slop
        // running 2.6.18-8.el5.
        // Works fine on tomacco running 2.6.9-42.0.2.ELsmp and
        // works fine on scum running 2.6.9-trick
        inet_ntop(AF_INET, h->h_addr_list[0], addr_list, (socklen_t) sizeof(addr_list));        // dpanter
        fprintf(stderr, "%s: Initializing client for '%s' (IP : %s) \n", __FILE__, h->h_name, addr_list);
    }

    udp_client_device->remoteServAddr.sin_family = h->h_addrtype;
    memcpy((char *) &udp_client_device->remoteServAddr.sin_addr.s_addr, h->h_addr_list[0], (size_t) h->h_length);
    udp_client_device->remoteServAddr.sin_port = htons(udp_client_device->port);

    udp_client_device->client_id = 0;
    strcpy(udp_client_device->client_tag, "<empty>");

    /* Set the byte info values (byte order and type sizes) */
    TRICK_GET_BYTE_ORDER(udp_client_device->byte_info[TC_BYTE_ORDER_NDX]);
    udp_client_device->byte_info[TC_LONG_SIZE_NDX] = (unsigned char) sizeof(long);

    udp_client_device->socket = the_socket;
    udp_client_device->socket_type = SOCK_DGRAM;

    /* Set the blocking flag for the connection */
    tc_blockio(udp_client_device, udp_client_device->blockio_type);

    return (TC_SUCCESS);
}
