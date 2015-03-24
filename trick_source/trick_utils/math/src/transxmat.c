/*
   PURPOSE: (Matrix transpose times Matrix)
   ASSUMPTIONS AND LIMITATIONS: ((Square matrix))
   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1993) (v1.0) (Init Release)))
 */

#include "../include/trick_math.h"

void transxmat(double **prod,   /* Out: Product of the two matrices */
               double **mat1,   /* In: Matrix 1 */
               double **mat2,   /* In: Matrix 2 */
               int n)
{                                      /* In: Array size */

    int i, j, k;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            prod[i][j] = 0.0;
            for (k = 0; k < n; k++) {
                prod[i][j] += mat1[k][i] * mat2[k][j];
            }
        }
    }

    return;

}
