/*
   PURPOSE: (Matrix transpose times vector)
   ASSUMPTIONS AND LIMITATIONS: ((Square matrix))
   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1993) (v1.0) (Init Release)))
 */

#include "trick/trick_math.h"

void transxvec(double *prod,    /* Out: Product of the two matrices */
               double **mat,    /* In: Matrix 1 */
               double *vec,     /* In: Matrix 2 */
               int n)
{                                      /* In: Array size */
    int i, k;

    for (i = 0; i < n; i++) {
        prod[i] = 0.0;
        for (k = 0; k < n; k++) {
            prod[i] += mat[k][i] * vec[k];
        }
    }

    return;
}
