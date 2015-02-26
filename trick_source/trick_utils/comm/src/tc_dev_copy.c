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
 * $Id: tc_dev_copy.c 1582 2011-05-05 20:30:21Z lin $
 */

#include <string.h>
#include "../include/tc.h"
#include "../include/tc_proto.h"

int tc_dev_copy(TCDevice * dest, TCDevice * src)
{
    /* 
     * If no destination memory or src memory
     */
    if (dest == NULL || src == NULL) {
        return (-1);
    }

    *dest = *src;

    return (0);

}
