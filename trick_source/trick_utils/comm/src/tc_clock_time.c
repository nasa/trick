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
 *  $Id: tc_clock_time.c 1582 2011-05-05 20:30:21Z lin $
 */

/* 
 * Get elapsed time since last call to tc_clock_init()
 */

#include "../include/tc.h"
#include "../include/tc_proto.h"

double tc_clock_time(double ref_time)
{
    double y_secs;

#if __WIN32__
    y_secs = GetTickCount();
#elif __linux
    struct timespec tp;

    /* Get absolute time from system */
    clock_gettime(CLOCK_REALTIME, &tp);

    /* Convert nanosecond portion to seconds and add to seconds portion */
    y_secs = (double) (tp.tv_sec) + ((double) (tp.tv_nsec) / 1000000000.0);
#else
    struct timeval tp;

    /* Get absolute time from system */
    gettimeofday(&tp, (struct timezone *) NULL);

    /* Convert microsecond portion to seconds and add to seconds portion */
    y_secs = (double) (tp.tv_sec) + ((double) (tp.tv_usec) / 1000000.0);
#endif

    /* Return the delta between the current time and the reference time */
    return (y_secs - ref_time);

}
