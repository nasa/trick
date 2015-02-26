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
 *  $Id: tc_listen.c 1582 2011-05-05 20:30:21Z lin $
 */

/* 
 * Check to see if a client is attempting to connect
 */

#include "../include/tc.h"
#include "../include/tc_proto.h"

#if (__APPLE__ | __WIN32__| __Lynx__ | __QNX__ | __ghs )

#include <sys/types.h>
#ifdef __APPLE__
#include <sys/time.h>
#include <unistd.h>
#endif

int tc_listen(TCDevice * listen_device)
{
    int status;

    fd_set chkset;
    struct timeval zero_time = { 0, 0 };

    /* Clear check file descriptor set */
    FD_ZERO(&chkset);

    /* Place device file descriptor into check file desciptor set */
    FD_SET(listen_device->socket, &chkset);

    /* Determine if the file descriptor has data to read */
    status = select(listen_device->socket + 1, &chkset, (fd_set *) NULL, (fd_set *) NULL, &zero_time);

    return (status);

}

#else

#include <poll.h>

int tc_listen(TCDevice * listen_device)
{
    int status;

    struct pollfd pfd;

    pfd.fd = listen_device->socket;
    pfd.events = POLLIN;
    pfd.revents = 0;
    status = poll(&pfd, (nfds_t) 1, 0);

    return (status);

}
#endif
