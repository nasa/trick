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
   PURPOSE: (Normalize a quaternion)

   REFERENCE: ((Flight Control - Ascent Flight Phase) (FSSR STS 83-0008A, Section 4.7.11 QUAT_NORM) (Volume 1, June 30, 
   1985))

   PROGRAMMERS: (((M Schira) (McDonnell Douglas) (Jan 1993) (v1.0) (Init Release)))

 */

/* 
 * $Id: quat_norm.c 49 2009-02-02 22:37:59Z lin $
 */

#include "../include/trick_math.h"


void quat_norm(double q2[4],    /* Out: quaternion */
               double q1[4])
{                                      /* In: quaternion */

    double coef;

    coef = q1[0] * q1[0] + q1[1] * q1[1] + q1[2] * q1[2] + q1[3] * q1[3];

    if (fabs(1.0 - coef) < 0.00000023842)
        coef = copysign(2.0, q1[0]) / (1.0 + coef);
    else
        coef = copysign(1.0, q1[0]) / sqrt(coef);

    q2[0] = coef * q1[0];
    q2[1] = coef * q1[1];
    q2[2] = coef * q1[2];
    q2[3] = coef * q1[3];

    return;
}
