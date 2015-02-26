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
   PURPOSE: (Invert matrix using LU Decomposition method)

   REFERENCE: ((Numerical Recipes [FORTRAN]))

   PROGRAMMERS: (((M Schira) (McDonnell Douglas) (Jan 1993) (v1.0) (Init Release))) */

/* 
 * $Id: LUD_inv.c 49 2009-02-02 22:37:59Z lin $
 */

#include <stdio.h>
#include <string.h>                    // for memcpy
#include <stdlib.h>                    // for malloc
#include "../include/trick_math.h"

int LUD_inv(                    /* RETURN: -- Zero */
               double **y,      /* Out: Matrix inverse */
               double **a,      /* In: Matrix */
               int n,           /* In: Matrix dimension */
               int *indx,       /* Inout: Temporary storage */
               double *ycol)
{                                      /* In: Temporary storage */
    int i, j;
    double d = 1.0;
    int iverr;
    double **work_matrix;
    double *mdata;

    // Duplicate the input matrix because LU_dcmp alters
    // the matrix passed to it. 
    work_matrix = malloc(n * sizeof(double *));
    mdata = (double *) malloc(n * n * sizeof(double));
    for (i = 0; i < n; i++) {
        work_matrix[i] = &mdata[i * n];
        memcpy(work_matrix[i], a[i], n * sizeof(double));
    }

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            y[i][j] = 0.0;
        }
        y[i][i] = 1.0;
    }

    iverr = LU_dcmp(work_matrix, d, n, indx, ycol);

    if (iverr == 0) {
        for (j = 0; j < n; j++) {
            for (i = 0; i < n; i++) {
                ycol[i] = y[i][j];
            }
            LU_bksb(ycol, work_matrix, n, indx);
            for (i = 0; i < n; i++) {
                y[i][j] = ycol[i];
            }
        }
    }
    // Free the work matrix
    free(mdata);
    free(work_matrix);

    return (iverr);
}
