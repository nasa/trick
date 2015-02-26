/*******************************************************************************
*                                                                              *
* Trick Simulation Environment Software                                        *
*                                                                              *
* Copyright (c) 1996,1997 LinCom Corporation, Houston, TX                      *
* All rights reserved.                                                         *
*                                                                              *
* Copyrighted by LinCom Corporation and proprietary to it. Any unauthorized    *
* use of Trick Software including source code, object code or executables is   *
* strictly prohibited and LinCom assumes no liability for such actions or      *
* results thereof.                                                             *
*                                                                              *
* Trick Software has been developed under NASA Government Contracts and        *
* access to it may be granted for Government work by the following contact:    *
*                                                                              *
* Contact: Charles Gott, Branch Chief                                          *
*          Simulation and Graphics Branch                                      *
*          Automation, Robotics, & Simulation Division                         *
*          NASA, Johnson Space Center, Houston, TX                             *
*                                                                              *
*******************************************************************************/
/* 
   PURPOSE: (Householder reduction of symmetric matrix to tridiagonal form)

   REFERENCE: ((Numerical Recipes [FORTRAN]))

   ASSUMPTIONS AND LIMITATIONS: ((Input matrix a is symmetric) (Input matrix a is destroyed))

   PROGRAMMERS: (((M Schira) (MDSS) (Jan 1994) (v1.0) (Init Release))) */

/* 
 * $Id: eigen_hh_red.c 49 2009-02-02 22:37:59Z lin $
 */

#include "../include/trick_math.h"


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
