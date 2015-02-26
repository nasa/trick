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
   PURPOSE: (Vector scaling) ASSUMPTIONS AND LIMITATIONS: ((3x1 vector implementation)) PROGRAMMERS: (((Les Quiocho)
   (NASA/JSC) (Jan 1990) (v1.0) (Init Release))) */

/* 
 * $Id: dv_scale.c 49 2009-02-02 22:37:59Z lin $
 */

#include "../include/trick_math.h"

void dv_scale(double prod[],    /* Out: Produce of the vector and the scalar */
              double vect[],    /* In: Vector */
              double scalar)
{                                      /* In: Scalar multiplier */
    prod[0] = vect[0] * scalar;
    prod[1] = vect[1] * scalar;
    prod[2] = vect[2] * scalar;

    return;
}
