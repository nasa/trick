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
 * $Id: tc_pending.c 1582 2011-05-05 20:30:21Z lin $
 */

/* 
 * See if data is available for reading on a non-blocking connection
 */

#include "../include/tc.h"
#include "../include/tc_proto.h"

int tc_pending(TCDevice * device)
{
    int n_read;

    if (!device) {
        TrickErrorHndlr *temp_error_hndlr = NULL;
        trick_error_report(temp_error_hndlr,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__, "Trying to call tc_pending on a NULL device");
        return (-1);
    }

    /* Use ioctl to check the number of bytes to read */
    IOCTL_SOCKET(device->socket, (unsigned long) FIONREAD, &n_read);

    return (n_read);

}
