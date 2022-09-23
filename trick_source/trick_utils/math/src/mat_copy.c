/*
   PURPOSE: (Generic matrix copy)
   ASSUMPTIONS AND LIMITATIONS: ((MxN matrix implementation))
   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (March 1993) (--) (Initial Release)))
 */

#include <stdio.h>
#include "trick/trick_math.h"

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
