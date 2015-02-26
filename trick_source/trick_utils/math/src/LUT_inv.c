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
   PURPOSE: (Invert lower triangular matrix)

   ASSUMPTIONS AND LIMITATIONS: ((Input matrix is lower triangular))

   PROGRAMMERS: (((M Schira) (McDonnell Douglas) (Feb 1994) (v1.0) (Init Release))) */

/* 
 * $Id: LUT_inv.c 49 2009-02-02 22:37:59Z lin $
 */

#include "../include/trick_math.h"

int LUT_inv(                    /* RETURN: -- Zero */
               double **y,      /* OUT: -- Matrix inverse */
               double **a,      /* IN: -- Matrix */
               int n)
{                                      /* IN: -- Matrix dimension */
    int i, j, k;
    int iverr;

    iverr = 0;
    for (i = 0; i < n; i++)
        if (a[i][i] == 0.0)
            iverr = 1;

    if (iverr == 0) {
        for (i = 0; i < n; i++)
            y[i][i] = 1.0 / a[i][i];

        for (i = 1; i < n; i++) {
            for (j = 0; j < i; j++) {
                y[i][j] = 0.0;
                for (k = j; k < i; k++) {
                    y[i][j] -= a[i][k] * y[k][j];
                }
                y[i][j] /= a[i][i];
            }
        }
    }

    return (iverr);
}
