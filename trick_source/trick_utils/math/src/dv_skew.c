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
   PURPOSE: (Vector transfomation to skew symmetric form) ASSUMPTIONS AND LIMITATIONS: ((3x1 vector implementation))
   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1990) (v1.0) (Init Release))) */

/* 
 * $Id: dv_skew.c 49 2009-02-02 22:37:59Z lin $
 */

#include "../include/trick_math.h"

void dv_skew(double skew[3][3], /* Out: Skew symmetic matrix */
             double vect[])
{                                      /* In: Vector to be transformed */
    skew[0][0] = 0.0;
    skew[1][1] = 0.0;
    skew[2][2] = 0.0;
    skew[0][1] = -vect[2];
    skew[0][2] = vect[1];
    skew[1][0] = vect[2];
    skew[1][2] = -vect[0];
    skew[2][0] = -vect[1];
    skew[2][1] = vect[0];

    return;
}
