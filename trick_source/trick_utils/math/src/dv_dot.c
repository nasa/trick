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
   PURPOSE: (Vector dot product) ASSUMPTIONS AND LIMITATIONS: ((3x1 vector implementation)) PROGRAMMERS: (((Les
   Quiocho) (NASA/JSC) (Jan 1990) (v1.0) (Init Release))) */

/* 
 * $Id: dv_dot.c 49 2009-02-02 22:37:59Z lin $
 */

#include "../include/trick_math.h"

double dv_dot(                  /* Return: Scalar dot or inner product */
                 double vect1[],        /* In: Vector 1 */
                 double vect2[])
{                                      /* In: Vector 2 */
    return (vect1[0] * vect2[0] + vect1[1] * vect2[1] + vect1[2] * vect2[2]);
}
