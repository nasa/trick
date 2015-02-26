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
 *  $Id: tc_isValid.c 3368 2013-12-19 22:34:06Z jpenn1 $
 */

/* 
 * Check to see if a device is valid
 */

#include "../include/tc.h"
#include "../include/tc_proto.h"

int tc_isValid(TCDevice * device)
{
    int nread;
    char one_byte;
    int ret = 0;

    if (device && (device->socket != TRICKCOMM_INVALID_SOCKET)) {

        tc_errno = 0;
        do {
            nread = recv(device->socket, &one_byte, sizeof(char),
                         MSG_PEEK | MSG_DONTWAIT);
        } while ((nread == -1) && (tc_errno == TRICKCOMM_EINTR));

        /* Return 0 or 1 based on the response from recv() */
        if ((nread <= 0) && (tc_errno != TRICKCOMM_EAGAIN)) {

            ret = 0; /* The socket is not valid */
        } 
        else {

            ret = 1; /* The socket is valid */
        }
    }

    return (ret);

}/* end tc_isValid */
