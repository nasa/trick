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
 * $Id: tc_error.c 1582 2011-05-05 20:30:21Z lin $
 */

/* 
 * Turn error reporting On/Off  
 */

#include "../include/tc.h"
#include "../include/tc_proto.h"

int tc_error(TCDevice * device, int on_off)
{
    size_t size;

    if (!device) {
        TrickErrorHndlr *temp_error_hndlr = NULL;
        trick_error_report(temp_error_hndlr,
                           TRICK_ERROR_ALERT, __FILE__, __LINE__, "Trying to set up error handler on NULL device");
        return (-1);
    }

    /* If there is no error handler, initialize one */
    if (device->error_handler == NULL) {
        size = sizeof(TrickErrorHndlr);
        device->error_handler = (TrickErrorHndlr *) malloc(size);
        trick_error_init(device->error_handler, NULL, NULL, TRICK_ERROR_ALL);
    }

    /* Now turn on/off */
    if (on_off == 1) {
        device->error_handler->report_level = TRICK_ERROR_ALL;
    } else {
        device->error_handler->report_level = TRICK_ERROR_SILENT;
    }

    return 0;

}
