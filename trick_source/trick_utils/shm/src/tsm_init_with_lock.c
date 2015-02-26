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
 *  $Id: tc_init.c 2296 2012-04-10 14:25:09Z dbankier $
 */

/* 
 * Initialize a shared memory segment containing a pthread read/write lock
 * and initialize the lock to be shared amongst multiple processes.
 * The user can then use it to lock memory during read/write operations.
 */
#include "../include/tsm.h"
#include "../include/tsm_proto.h"


int tsm_init_with_lock(TSMDevice * shm_device)
{
    int ret;

    shm_device->use_lock = 1;
    ret = tsm_init(shm_device);
    return (ret);

}
