/******************************************************************************
*                                                                             *
* Trick Simulation Environment Software                                       *
*                                                                             *
* Copyright (c) 1996,1997 LinCom Corporation, Houston, TX                     *
* All rights reserved.                                                        *
*                                                                             *
* Copyrighted by LinCom Corporation and proprietary to it. Any unauthorized   *
* use of Trick Software including source code, object code or executables is  *
* strictly prohibited and LinCom assumes no liability for such actions or     *
* results thereof.                                                            *
*                                                                             *
* Trick Software has been developed under NASA Government Contracts and       *
* access to it may be granted for Government work by the following contact:   *
*                                                                             *
* Contact: Charles Gott, Branch Chief                                         *
*          Flight Robotic Systems Branch                                      *
*          Automation, Robotics, & Simulation Division                        *
*          NASA, Johnson Space Center, Houston, TX                            *
*                                                                             *
******************************************************************************/

/* 
 * $Id: tc_blockio.c 2002 2011-10-20 21:05:12Z marnold $
 */

#include "../include/tc.h"
#include "../include/tc_proto.h"

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
