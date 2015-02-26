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
    (This header file defines the "RELEASE()" macro for supported platforms.
     The release function should yield the processor from the processes that
     makes the call. This allows the UNIX schedular to arbitrate process
     priorities during spinloops based on the status of processes waiting 
     to run)
REFERENCE:
    ((( Bailey R. & Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC / Engineering Directorate / Automation and Robotics Division)
      (June 1994) (--)))
PROGRAMMER:
    (((Eddie J. Paddock ) (LinCom) (2/96) (--) (Realtime))
     ((Scott Killingsworth ) (LinCom) (October 1997) (OSF1 port) (Issue #1233)))
*/

/*
 * $Id: release.h 3031 2013-06-18 14:37:55Z alin $
 */


#ifndef RELEASE_H
#define RELEASE_H

#if __sgi 

#define RELEASE() sginap(0)
#define RELEASE_1() sginap(1)

#elif __vxworks

#define RELEASE() taskDelay(1)
#define RELEASE_1() taskDelay(1)

#else

#include <time.h>

#define RELEASE() { struct timespec timeout;\
                    timeout.tv_sec = 0;\
                    timeout.tv_nsec = 0;\
                    nanosleep(&timeout, NULL);}

#define RELEASE_1() RELEASE()
#endif
#endif
