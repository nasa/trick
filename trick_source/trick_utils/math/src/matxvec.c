/*
   PURPOSE: (Matrix times vector)
   ASSUMPTIONS AND LIMITATIONS: ((Square matrix))
   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1993) (v1.0) (Init Release))) */

#include "trick/trick_math.h"

void matxvec(double *prod,      /* Out: product of the two matrices */
             double **mat,      /* In: matrix 1 */
             double *vec,       /* In: matrix 2 */
             int n)
{                                      /* In: array size */

    int i, k;

    for (i = 0; i < n; i++) {
        prod[i] = 0.0;
        for (k = 0; k < n; k++) {
            prod[i] += mat[i][k] * vec[k];
        }
    }

    return;

}
