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
   PURPOSE: (Vector print) REFERENCE: ((See also: source file name)) PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1990) 
   (v1.0) (Init Release))) */

/* 
 * $Id: vec_print.c 49 2009-02-02 22:37:59Z lin $
 */

#include <stdio.h>
#include "../include/trick_math.h"


void vec_print(double *vec,     /* In: Matrix to be printed */
               int n)
{                                      /* In: Array size */
    int i;

    for (i = 0; i < n; i++) {
        fprintf(stderr, " %16.12f ", vec[i]);
        fprintf(stderr, "\n");
    }

    return;
}
