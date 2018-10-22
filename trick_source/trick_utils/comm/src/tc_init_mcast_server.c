/*
   PURPOSE: (Set up device for multicast communication as a server.)

   REFERENCE: ((None))

   ASSUMPTIONS AND LIMITATIONS:
       ((mcast_server_device->port must be specified.)
        (mcast_server_device->mcast_group must be specified.)
        (mcast_server_device->blockio_type should be specified; defaults to blocking.))

   CLASS: ()

   LIBRARY DEPENDENCY: ((tc_init_mcast_server.o))

   PROGRAMMERS: (((M. Will) (L3-Com) (29 November 2006) (--) (CAIL))) */

#include "trick/tc.h"
#include "trick/tc_proto.h"
#include "trick/trick_byteswap.h"

int tc_init_mcast_server(TCDevice * mcast_server_device)
{

    struct sockaddr_in sockin;
    struct ip_mreq mreq;
    const unsigned int yes = 1;

    if (!mcast_server_device) {
        trick_error_report(mcast_server_device->error_handler,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__, "mcast device is null.");
        return (-1);
    }

    /* Open a UDP/Multicast socket */
    mcast_server_device->socket = socket(AF_INET, SOCK_DGRAM, 0);

    if (mcast_server_device->socket == TRICKCOMM_INVALID_SOCKET) {
        perror("tc_init_mcast_server: socket call failed");

        trick_error_report(mcast_server_device->error_handler,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__, "Error creating mcast socket.");

        return (TRICKCOMM_INVALID_SOCKET);
    }

    /* Allow socket's bound address to be used by others */
    if (setsockopt(mcast_server_device->socket, SOL_SOCKET, SO_REUSEADDR, &yes, (socklen_t) sizeof(yes)) < 0) {
        perror("tc_init_mcast_server.c: SO_REUSEADDR error");

        trick_error_report(mcast_server_device->error_handler,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__,
                           "Could not allow multiple sockets to use the same port = \n", (mcast_server_device->port));

        return (-1);
    }

    sockin.sin_addr.s_addr = INADDR_ANY;
    sockin.sin_family = TRICKCOMM_SOCKET_FAMILY;
    sockin.sin_port = htons((short) (mcast_server_device->port));

    if (bind(mcast_server_device->socket, (struct sockaddr *) &sockin, (socklen_t) sizeof(sockin)) < 0) {
        perror("tc_init_mcast_server: bind error");

        trick_error_report(mcast_server_device->error_handler,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__,
                           "Could not bind socket for port %d\n", (mcast_server_device->port));

        return (TC_COULD_NOT_BIND_SOCKET);
    }

    mreq.imr_multiaddr.s_addr = inet_addr(mcast_server_device->mcast_group);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    /* Request that the kernel join a multicast group */
    if (setsockopt(mcast_server_device->socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, (socklen_t) sizeof(mreq)) < 0) {
        perror("tc_init_mcast_server: setsockopt to join multicast group");

        trick_error_report(mcast_server_device->error_handler,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__,
                           "Error joining multicast group %s.", mcast_server_device->mcast_group);

        return (TC_COULD_NOT_ADD_MEMBERSHIP);
    }

    /* Set blocking, anticipating that the server will want to wait for incoming messages */
    mcast_server_device->blockio_type = TC_COMM_BLOCKIO;
    mcast_server_device->client_id = 0;
    strcpy(mcast_server_device->client_tag, "<empty>");

    /* Set the byte info values (byte order and type sizes) */
    TRICK_GET_BYTE_ORDER(mcast_server_device->byte_info[TC_BYTE_ORDER_NDX]);
    mcast_server_device->byte_info[TC_LONG_SIZE_NDX] = (unsigned char) sizeof(long);

    return (TC_SUCCESS);

}
