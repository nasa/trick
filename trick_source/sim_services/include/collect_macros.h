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
    (Macros useful for collect mechanism of S_define)
PROGRAMMER:
    (((Robert W. Bailey) (LinCom) (7/95) (--) (Realtime)))
*/

/*
 * $Log: collect_macros.h,v $
 * Revision 5.4  2005-03-08 14:43:55-06  vetter
 * 8 space indent all code
 *
 * Revision 5.3  2005-02-11 09:23:49-06  lin
 * NUM_COLLECT macro nees a cast
 *
 * Revision 5.2  2005-01-31 10:54:33-06  lin
 * NUM_COLLECT macro is expensive
 *
 * Revision 5.1  2004-08-05 13:08:45-05  lin
 * Bump
 *
 * Revision 4.2  2004/01/16 21:19:23  lin
 * Add a log to all files
 *
 */

#include "exec_proto.h"

#ifndef COLLECT_MACROS_H
#define COLLECT_MACROS_H

#define NUM_COLLECT(X) (( X == 0 ) ? 0 : *((long *)X - 1))

#endif
