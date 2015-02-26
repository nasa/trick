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
 * $Id: tc_set_blockio.c 1582 2011-05-05 20:30:21Z lin $
 */

/* 
 * Set the time in seconds that a blocking read will wait before timing out
 */

#include "../include/tc.h"
#include "../include/tc_proto.h"

int tc_set_blockio_timeout_limit(TCDevice * device, double limit)
{

    if (!device) {
        TrickErrorHndlr *temp_error_hndlr = NULL;
        trick_error_report(temp_error_hndlr,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__,
                           "Trying to tc_set_blockio_timeout_limit " "to a NULL device");
        return (-1);
    }

    trick_error_report(device->error_handler,
                       TRICK_ERROR_ALL, __FILE__, __LINE__,
                       "set blockio_timeout_limit for client "
                       "(ID = %d  tag = %s) limit = %f\n", device->client_id, device->client_tag, limit);

    device->blockio_limit = limit;

    return 0;

}
