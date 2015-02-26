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
   PURPOSE: (Compute the orthonormal transformation matrix from the corresponding normalized quaternion)

   PROGRAMMERS: (((M Schira) (McDonnell Douglas) (Jan 1993) (v1.0) (Init Release)))

 */

/* 
 * $Id: quat_to_mat.c 49 2009-02-02 22:37:59Z lin $
 */

#include "../include/trick_math.h"

void quat_to_mat(double a[3][3],        /* Out: transformation matrix */
                 double quat[4])
{                                      /* In: quaternion */

    a[0][0] = 1. - 2. * (quat[2] * quat[2] + quat[3] * quat[3]);
    a[1][1] = 1. - 2. * (quat[3] * quat[3] + quat[1] * quat[1]);
    a[2][2] = 1. - 2. * (quat[1] * quat[1] + quat[2] * quat[2]);
    a[1][0] = 2. * (quat[1] * quat[2] + quat[0] * quat[3]);
    a[0][1] = 2. * (quat[1] * quat[2] - quat[0] * quat[3]);
    a[2][0] = 2. * (quat[1] * quat[3] - quat[0] * quat[2]);
    a[0][2] = 2. * (quat[1] * quat[3] + quat[0] * quat[2]);
    a[2][1] = 2. * (quat[2] * quat[3] + quat[0] * quat[1]);
    a[1][2] = 2. * (quat[2] * quat[3] - quat[0] * quat[1]);

    return;
}
