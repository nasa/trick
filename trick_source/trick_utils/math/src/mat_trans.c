/*
   PURPOSE: (Matrix transpose)
   ASSUMPTIONS AND LIMITATIONS: ((Square matrix))
   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1993) (v1.0) (Init Release))) */

#include "trick/trick_math.h"

void mat_trans(double **trans,  /* Out: Product of the two matrices */
               double **mat,    /* In: Matrix 1 */
               int n)
{                                      /* In: Array size */
    int i, j;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            trans[i][j] = mat[j][i];
        }
    }

    return;
}
