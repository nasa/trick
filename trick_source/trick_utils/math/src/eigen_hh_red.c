/*
   PURPOSE: (Householder reduction of symmetric matrix to tridiagonal form)

   REFERENCE: ((Numerical Recipes [FORTRAN]))

   ASSUMPTIONS AND LIMITATIONS: ((Input matrix a is symmetric) (Input matrix a is destroyed))

   PROGRAMMERS: (((M Schira) (MDSS) (Jan 1994) (v1.0) (Init Release))) */

#include "trick/trick_math.h"


void eigen_hh_red(double **a,   /* In: Input matrix */
                  int n,        /* In: Array size */
                  double *d,    /* Out: Diagonal elements */
                  double *e)
{                                      /* Out: Off diagonal elements */
    int i, j, k, l;
    double f, g, h, hh, scale;

    if (n > 1) {
        for (i = n - 1; i >= 1; i--) {
            l = i - 1;
            h = 0.0;
            scale = 0.0;
            if (l > 0) {
                for (k = 0; k <= l; k++)
                    scale += fabs(a[i][k]);
                if (scale == 0.0)
                    e[i] = a[i][l];
                else {
                    for (k = 0; k <= l; k++) {
                        a[i][k] /= scale;
                        h += a[i][k] * a[i][k];
                    }
                    f = a[i][l];
                    g = -copysign(sqrt(h), f);
                    e[i] = scale * g;
                    h -= f * g;
                    a[i][l] = f - g;
                    f = 0.0;
                    for (j = 0; j <= l; j++) {
                        a[j][i] = a[i][j] / h;
                        g = 0.0;
                        for (k = 0; k <= j; k++)
                            g += a[j][k] * a[i][k];
                        if (l > j) {
                            for (k = j + 1; k <= l; k++)
                                g += a[k][j] * a[i][k];
                        }
                        e[j] = g / h;
                        f += e[j] * a[i][j];
                    }
                    hh = f / (h + h);
                    for (j = 0; j <= l; j++) {
                        f = a[i][j];
                        g = e[j] - hh * f;
                        e[j] = g;
                        for (k = 0; k <= j; k++)
                            a[j][k] -= (f * e[k] + g * a[i][k]);
                    }
                }
            } else {
                e[i] = a[i][l];
            }
            d[i] = h;
        }
    }
    d[0] = 0.0;
    e[0] = 0.0;
    for (i = 0; i < n; i++) {
        l = i - 1;
        if (d[i] != 0.0) {
            for (j = 0; j <= l; j++) {
                g = 0.0;
                for (k = 0; k <= l; k++)
                    g += a[i][k] * a[k][j];
                for (k = 0; k <= l; k++)
                    a[k][j] -= g * a[k][i];
            }
        }
        d[i] = a[i][i];
        a[i][i] = 1.0;
        if (l >= 0) {
            for (j = 0; j <= l; j++) {
                a[i][j] = 0.0;
                a[j][i] = 0.0;
            }
        }
    }

    return;
}
