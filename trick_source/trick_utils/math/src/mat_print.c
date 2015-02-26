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
   PURPOSE: (Matrix print) ASSUMPTIONS AND LIMITATIONS: ((Square Matrix)) PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan
   1993) (v1.0) (Init Release))) */

/* 
 * $Id: mat_print.c 49 2009-02-02 22:37:59Z lin $
 */

#include <stdio.h>
#include "../include/trick_math.h"

void mat_print(double **mat,    /* In: Matrix to be printed */
               int n)
{                                      /* In: Array size */
    int i, j;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            fprintf(stderr, " %16.12f ", mat[i][j]);
        }
        fprintf(stderr, "\n");
    }

    return;
}
