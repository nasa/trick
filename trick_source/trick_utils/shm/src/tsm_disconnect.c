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
 * Disconnect (detach) a shared memory segment
 */
#include "../include/tsm.h"
#include "../include/tsm_proto.h"


int tsm_disconnect(TSMDevice * shm_device)
{
    int ret;

    ret = TSM_SUCCESS;

    // if we had a read/write lock, destroy it first
    if (shm_device->rwlock_addr != NULL) {
        ret = pthread_rwlockattr_destroy(&shm_device->rwlattr);
        if (ret == -1) {
            perror("tsm_disconnect SHARED MEMORY DESTROY LOCK ATTRIBUTES FAILED");
        }
        ret = pthread_rwlock_destroy(shm_device->rwlock_addr);
        if (ret == -1) {
            perror("tsm_disconnect SHARED MEMORY DESTROY LOCK FAILED");
        }
    }

    ret = shmdt(shm_device->addr);
    if (ret == -1) {
        perror("tsm_disconnect SHARED MEMORY DETACH FAILED");
    }

    return (ret);
}
