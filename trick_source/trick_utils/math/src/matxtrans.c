/*
   PURPOSE: (Matrix times Matrix transpose)
   ASSUMPTIONS AND LIMITATIONS: ((Square matrix))
   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1993) (v1.0) (Init Release))) */

#include "../include/trick_math.h"

void matxtrans(double **prod,   /* Out: product of the two matrices */
               double **mat1,   /* In: matrix 1 */
               double **mat2,   /* In: matrix 2 */
               int n)
{                                      /* In: array size */

    int i, j, k;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            prod[i][j] = 0.0;
            for (k = 0; k < n; k++) {
                prod[i][j] += mat1[i][k] * mat2[j][k];
            }
        }
    }

    return;

}
