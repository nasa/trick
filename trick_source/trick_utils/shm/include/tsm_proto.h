/*******************************************************************************
*                                                                              *
* Trick Simulation Environment Software                                        *
*                                                                              *
* Copyright (c) 1996,1997 LinCom Corporation, Houston, TX                      *
* All rights reserved.                                                         *
*                                                                              *
* Copyrighted by LinCom Corporation and proprietary to it. Any unauthorized    *
* use of Trick Software including source code, object code or executables is   *
* strictly prohibited and LinCom assumes no liability for such actions or      *
* results thereof.                                                             *
*                                                                              *
* Trick Software has been developed under NASA Government Contracts and        *
* access to it may be granted for Government work by the following contact:    *
*                                                                              *
* Contact: Charles Gott, Branch Chief                                          *
*          Simulation and Graphics Branch                                      *
*          Automation, Robotics, & Simulation Division                         *
*          NASA, Johnson Space Center, Houston, TX                             *
*                                                                              *
*******************************************************************************/
/*
PURPOSE:
     (Specify the trick shared memory library API and structure definition)
PROGRAMMERS:
     (((Danny Strauss) (L3) (Sep 2012) (--) (Initial Version)))
ICG:
     (No)
*/

/*
 *  $Id: tc_proto.h 1582 2011-05-05 20:30:21Z lin $
 */

#ifndef _tsm_proto_h
#define _tsm_proto_h

#include "tsm.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize a shared memory segment */
int tsm_init(TSMDevice * device);

/* Initialize a shared memory segment containing a pthread lock */
int tsm_init_with_lock(TSMDevice * device);

/* Reconnect to a disconnected shared memory segment */
int tsm_reconnect(TSMDevice * device);

/* Disconnect (detach) a shared memory segment */
int tsm_disconnect(TSMDevice * device);

#ifdef __cplusplus
}
#endif
#endif
