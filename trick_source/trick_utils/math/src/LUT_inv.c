/*
   PURPOSE: (Invert lower triangular matrix)

   ASSUMPTIONS AND LIMITATIONS: ((Input matrix is lower triangular))

   PROGRAMMERS: (((M Schira) (McDonnell Douglas) (Feb 1994) (v1.0) (Init Release))) */

#include "trick/trick_math.h"

int LUT_inv(                    /* RETURN: -- Zero */
               double **y,      /* OUT: -- Matrix inverse */
               double **a,      /* IN: -- Matrix */
               int n)
{                                      /* IN: -- Matrix dimension */
    int i, j, k;
    int iverr;

    iverr = 0;
    for (i = 0; i < n; i++)
        if (a[i][i] == 0.0)
            iverr = 1;

    if (iverr == 0) {
        for (i = 0; i < n; i++)
            y[i][i] = 1.0 / a[i][i];

        for (i = 1; i < n; i++) {
            for (j = 0; j < i; j++) {
                y[i][j] = 0.0;
                for (k = j; k < i; k++) {
                    y[i][j] -= a[i][k] * y[k][j];
                }
                y[i][j] /= a[i][i];
            }
        }
    }

    return (iverr);
}
