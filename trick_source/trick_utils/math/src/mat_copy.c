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
   PURPOSE: (Generic matrix copy) ASSUMPTIONS AND LIMITATIONS: ((MxN matrix implementation)) PROGRAMMERS: (((Les
   Quiocho) (NASA/JSC) (March 1993) (--) (Initial Release))) */

/* 
 * $Id: mat_copy.c 49 2009-02-02 22:37:59Z lin $
 */

#include <stdio.h>
#include "../include/trick_math.h"

void mat_copy(                  /* Return: -- none */
                 double **copy, /* Out: copy of the matrix */
                 double **mat,  /* In: matrix to be copied */
                 int m,         /* In: row dimension */
                 int n)
{                                      /* In: column dimension */
    int i, j;

    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            copy[i][j] = mat[i][j];
        }
    }

    return;
}
