
#ifndef __WIN32__
#  include <sys/ioctl.h>
#  define IOCTL_SOCKET ioctl
#else
#  define IOCTL_SOCKET ioctlsocket
#endif

#include "trick/tc.h"
#include "trick/tc_proto.h"

/*
 *Set a device to BLOCKIO or NOBLOCKIO - may be used as a toggle
 */
int tc_blockio(TCDevice * device, TCCommBlocking blockflag)
{
    char client_str[TC_TAG_LENGTH + 256];
    int on = 1;
    int off = 0;

    if (device == NULL) {
        return TC_EWOULDBLOCK;
    }

    /* Status message */
    sprintf(client_str, "(ID = %d  tag = %s)", device->client_id, device->client_tag);
    trick_error_report(device->error_handler, TRICK_ERROR_TRIVIAL, __FILE__,
                       __LINE__, "%s blockflag = %d\n", client_str, blockflag);


    switch (blockflag) {

        case TC_COMM_BLOCKIO:
            /* Set socket to blocking */
            if (IOCTL_SOCKET(device->socket, (unsigned long) FIONBIO, &off) < 0) {
                trick_error_report(device->error_handler,
                                   TRICK_ERROR_ALERT, __FILE__, __LINE__, "could not set socket to blocking\n");
                return (TC_EWOULDBLOCK);
            }
            device->blockio_type = blockflag;
            break;

        case TC_COMM_NOBLOCKIO:
            /* Set socket to non-blocking */
            if (IOCTL_SOCKET(device->socket, (unsigned long) FIONBIO, &on) < 0) {
                trick_error_report(device->error_handler,
                                   TRICK_ERROR_ALERT, __FILE__, __LINE__, "could not set socket to non-blocking\n");
                return (TC_EWOULDBLOCK);
            }
            device->blockio_type = blockflag;
            break;

        case TC_COMM_TIMED_BLOCKIO:
            /* Set socket to non-blocking */
            if (IOCTL_SOCKET(device->socket, (unsigned long) FIONBIO, &on) < 0) {
                trick_error_report(device->error_handler,
                                   TRICK_ERROR_ALERT, __FILE__,
                                   __LINE__, "could not set socket to software-blocking\n");
                return (TC_EWOULDBLOCK);
            }
            device->blockio_type = blockflag;
            break;

        case TC_COMM_ALL_OR_NOTHING:
            /* Set socket to non-blocking */
            if (IOCTL_SOCKET(device->socket, (unsigned long) FIONBIO, &on) < 0) {
                trick_error_report(device->error_handler,
                                   TRICK_ERROR_ALERT, __FILE__, __LINE__, "could not set socket to all_or nothing\n");
                return (TC_EWOULDBLOCK);
            }
            device->blockio_type = blockflag;
            break;

        default:
            trick_error_report(device->error_handler,
                               TRICK_ERROR_ALERT, __FILE__, __LINE__,
                               "Invalid second argument."
                               "Second argument should be one of the following:\n"
                               "  TC_COMM_BLOCKIO,\n"
                               "  TC_COMM_NOBLOCKIO,\n" "  TC_COMM_TIMED_BLOCKIO,\n" "  TC_COMM_ALL_OR_NOTHING\n\n");
            return (TC_EWOULDBLOCK);
    }

    return (0);

}
