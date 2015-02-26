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
 *  $Id: tc_clock_init.c 1582 2011-05-05 20:30:21Z lin $
 */

/* 
 *  Get system clock reference time
 */

#include "../include/tc.h"
#include "../include/tc_proto.h"

double tc_clock_init(void)
{
    double ref_time;

#if __WIN32__
    ref_time = GetTickCount();
#elif __linux
    struct timespec tp;

    /* Get absolute time from system */
    clock_gettime(CLOCK_REALTIME, &tp);

    /* Convert nanosecond portion to seconds and add to seconds portion */
    ref_time = ((double)(tp.tv_sec)) + (((double)(tp.tv_nsec)) / 1000000000.0);
#else
    struct timeval tp;

    /* Get absolute time from system */
    gettimeofday(&tp, (struct timezone *) NULL);

    /* Convert microsecond portion to seconds and add to seconds portion */
    ref_time = (double) (tp.tv_sec) + ((double) (tp.tv_usec) / 1000000.0);
#endif

    return (ref_time);

}
