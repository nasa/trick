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
   PURPOSE: (Vector normalization)

   ASSUMPTIONS AND LIMITATIONS: ((3x1 vector implementation)) PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1990) (v1.0) 
   (Init Release))) */

/* 
 * $Id: dv_norm.c 49 2009-02-02 22:37:59Z lin $
 */

#include "../include/trick_math.h"

void dv_norm(double norm[3],    /* Out: Norm vector */
             double vect[3])
{                                      /* In: Vector to normalized */
    double square, magnitude;

    square = (vect[0] * vect[0] + vect[1] * vect[1] + vect[2] * vect[2]);
    if (square > 0.0) {
        magnitude = sqrt(square);
        norm[0] = vect[0] / magnitude;
        norm[1] = vect[1] / magnitude;
        norm[2] = vect[2] / magnitude;
    } else {
        norm[0] = 0.0;
        norm[1] = 0.0;
        norm[2] = 0.0;
    }

    return;
}
