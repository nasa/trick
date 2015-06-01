/*
   PURPOSE: (Perform LU decomposition of matrix a)

   REFERENCE: ((Numerical Recipes [FORTRAN]))

   PROGRAMMERS: (((M Schira) (McDonnell Douglas) (Jan 1993) (v1.0) (Init Release))) */

#include <stdio.h>
#include "trick/trick_math.h"

#define tiny 1.0e-20

int LU_dcmp(                    /* Return: Zero */
               double **a,      /* In: Matrix */
               double d,        /* Out: Determinant */
               int n,           /* In: Matrix dimension */
               int *indx,       /* Inout: Temporary storage */
               double *vv)
{                                      /* Inout: Temporary storage */
    int i, j, k;
    double dum, sum, aamax;
    int imax = 0, iverr;

    iverr = 0;
    d = 1.0;
    for (i = 0; i < n; i++) {
        aamax = 0.0;
        for (j = 0; j < n; j++) {
            if (fabs(a[i][j]) > aamax)
                aamax = fabs(a[i][j]);
        }
        if (aamax == 0.0) {
            iverr = 1;
            return (iverr);
        }
        vv[i] = 1.0 / aamax;
    }

    for (j = 0; j < n; j++) {
        if (j > 0) {
            for (i = 0; i < j; i++) {
                sum = a[i][j];
                if (i > 0) {
                    for (k = 0; k < i; k++) {
                        sum -= a[i][k] * a[k][j];
                    }
                    a[i][j] = sum;
                }
            }
        }
        aamax = 0.0;
        for (i = j; i < n; i++) {
            sum = a[i][j];
            if (j > 0) {
                for (k = 0; k < j; k++) {
                    sum -= a[i][k] * a[k][j];
                }
                a[i][j] = sum;
            }
            dum = vv[i] * fabs(sum);
            if (dum >= aamax) {
                imax = i;
                aamax = dum;
            }
        }
        if (j != imax) {
            for (k = 0; k < n; k++) {
                dum = a[imax][k];
                a[imax][k] = a[j][k];
                a[j][k] = dum;
            }
            d = -d;
            vv[imax] = vv[j];
        }
        indx[j] = imax;
        if (j != n - 1) {
            if (a[j][j] == 0.0)
                a[j][j] = tiny;
            dum = 1.0 / a[j][j];
            for (i = j + 1; i < n; i++) {
                a[i][j] = a[i][j] * dum;
            }
        }
    }
    if (a[n - 1][n - 1] == 0.0)
        a[n - 1][n - 1] = tiny;

    return (iverr);
}
