/*
   PURPOSE: (Invert matrix using LU Decomposition method)

   REFERENCE: ((Numerical Recipes [FORTRAN]))

   PROGRAMMERS: (((M Schira) (McDonnell Douglas) (Jan 1993) (v1.0) (Init Release))) */

#include <stdio.h>
#include <string.h>                    // for memcpy
#include <stdlib.h>                    // for malloc
#include "trick/trick_math.h"

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
    work_matrix = (double **) malloc(n * sizeof(double *));
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
