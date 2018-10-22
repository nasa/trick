/*
   PURPOSE: (Set up device for mcast communication as a client.)

   REFERENCE: ((None))

   ASSUMPTIONS AND LIMITATIONS:
      ((mcast_client_device->mcast_group is required as input.)
       (mcast_client_device->port is required as input.)
       (mcast_client_device->blockio_type should be specified; defaults to blocking.)
       (mcast_client_device->ttl must be specified for hops farther than the local host.))

   CLASS: ()

   LIBRARY DEPENDENCY: ((tc_init_mcast_client.o))

   PROGRAMMERS: (((M. Will) (L3-Com) (29 November 2006) (--) (CAIL)))
 */

#include "trick/tc.h"
#include "trick/tc_proto.h"
#include "trick/trick_byteswap.h"

int tc_init_mcast_client(TCDevice * mcast_client_device)
{
    if (!mcast_client_device) {
        trick_error_report(mcast_client_device->error_handler,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__, "mcast device is null.");
        return (-1);
    }

    /* Open a UDP/Multicast socket */
    mcast_client_device->socket = socket(AF_INET, SOCK_DGRAM, 0);

    if (mcast_client_device->socket == TRICKCOMM_INVALID_SOCKET) {
        perror("tc_init_mcast_client: socket creation error ");

        trick_error_report(mcast_client_device->error_handler,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__, "Error creating UDP multi-cast socket.");

        return (TC_COULD_NOT_OPEN_SOCKET);
    }

    /* Set up remoteServAddr for use in 'sendto' (tc_write) message to the server */
    memset((void *) &mcast_client_device->remoteServAddr, 0, sizeof(struct sockaddr_in));
    mcast_client_device->remoteServAddr.sin_family = AF_INET;
    mcast_client_device->remoteServAddr.sin_addr.s_addr = inet_addr(mcast_client_device->mcast_group);
    mcast_client_device->remoteServAddr.sin_port = htons(mcast_client_device->port);

    mcast_client_device->client_id = 0;
    strcpy(mcast_client_device->client_tag, "<empty>");

    /*
       As the values of the TTL field increase, routers will expand the number of hops they will forward a multicast
       packet. To provide meaningful scope control, multicast routers enforce the following "thresholds" on
       forwarding based on the TTL field:

       0 restricted to the same host 1 restricted to the same subnet 32 restricted to the same site 64 restricted to
       the same region 128 restricted to the same continent 255 unrestricted */
    if (setsockopt(mcast_client_device->socket, IPPROTO_IP, IP_MULTICAST_TTL,
                   &mcast_client_device->ttl, (socklen_t) sizeof(mcast_client_device->ttl)) < 0) {
        perror("tc_init_mcast_client: setsockopt for ttl failed");
        trick_error_report(mcast_client_device->error_handler,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__, "Unable to set time-to-live (ttl) hops.");
        return (TC_COULD_NOT_SET_TTL);
    }

    /* Set the byte info values (byte order and type sizes) */
    TRICK_GET_BYTE_ORDER(mcast_client_device->byte_info[TC_BYTE_ORDER_NDX]);
    mcast_client_device->byte_info[TC_LONG_SIZE_NDX] = (unsigned char) sizeof(long);

    return (TC_SUCCESS);
}
