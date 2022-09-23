/*
   PURPOSE: (Back substitute for solution to linear equations)

   REFERENCE: ((Numerical Recipes [FORTRAN]))

   ASSUMPTIONS AND LIMITATIONS: ((None))

   CLASS: (N/A)

   LIBRARY DEPENDENCY: ((none))

   PROGRAMMERS: (((M Schira) (McDonnell Douglas) (Jan 1993) (v1.0) (Init Release))) */

#include "trick/trick_math.h"

void LU_bksb(                   /* Return: Zero */
                double *b,      /* Inout: Temporary storage */
                double **a,     /* In: Matrix */
                int n,          /* In: Matrix dimension */
                int *indx)
{                                      /* Inout: Temporary storage */
    int i, j;
    double sum;
    int ii, ll;

    ii = -1;
    for (i = 0; i < n; i++) {
        ll = indx[i];
        sum = b[ll];
        b[ll] = b[i];
        if (ii != -1) {
            for (j = ii; j < i; j++) {
                sum -= a[i][j] * b[j];
            }
        } else if (sum != 0.0) {
            ii = i;
        }
        b[i] = sum;
    }

    for (i = n - 1; i > -1; i--) {
        sum = b[i];
        if (i < n - 1) {
            for (j = i + 1; j < n; j++) {
                sum -= a[i][j] * b[j];
            }
        }
        b[i] = sum / a[i][i];
    }
    return;
}
