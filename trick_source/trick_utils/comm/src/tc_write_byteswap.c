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
 * $Id: tc_write_byteswap.c 188 2009-05-14 18:44:36Z lin $
 */

/* 
 * Write data to a device and byteswap if server and client 
 * byte orders are different.
 */

#include "../include/tc.h"
#include "../include/tc_proto.h"

int tc_write_byteswap(TCDevice * device, char *buffer, int size, ATTRIBUTES * attr)
{
    char local_byteorder;
    int ret = 0;
    static char swap[65536];

    if (!device) {
        TrickErrorHndlr *temp_error_hndlr = NULL;
        trick_error_report(temp_error_hndlr,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__, "Trying to write_byteswap to a NULL device");
        return (-1);
    }

    TRICK_GET_BYTE_ORDER(local_byteorder)

        if (device->byte_info[TC_BYTE_ORDER_NDX] != local_byteorder) {
        memset(swap, 0, (size_t) size);
        trick_bswap_buffer(swap, buffer, attr, 1);
        ret = tc_write(device, (char *) swap, size);
        return (ret);
    } else {
        return (tc_write(device, (char *) buffer, size));
    }

    return 0;

}
