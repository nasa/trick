/*
   PURPOSE: (Eigenvalues and eigenvectors of symmetric matrix)

   REFERENCE: ((Numerical Recipes [FORTRAN]) (QL transformation with origin shifts))

   ASSUMPTIONS AND LIMITATIONS: ((Square matrix))

   PROGRAMMERS: (((M Schira) (MDSS) (Jan 1994) (v1.0) (Init Release))) */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "trick/trick_math.h"

void eigen_ql(double *d,        /* Inout: input diagonal elements, output eigenvalues */
              double *e,        /* In: off diagonal elements */
              int n,            /* In: array size */
              double **z)
{                                      /* Out: eigenvectors */
    int i, k, l, m, iter;
    double f, g, r, s, c, p, dd, b;

    if (n > 1) {
        for (i = 1; i < n; i++)
            e[i - 1] = e[i];
        e[n - 1] = 0.0;
        for (l = 0; l < n; l++) {
            iter = 0;
          label_1:
            for (m = l; m < n - 1; m++) {
                dd = fabs(d[m]) + fabs(d[m + 1]);
                if (fabs(e[m]) + dd == dd)
                    goto label_2;
            }
            m = n - 1;
          label_2:
            if (m != l) {
                if (iter == 30) {
                    fprintf(stderr, "30 iterations in tqli \n");
                    exit(0);
                }
                iter++;
                g = (d[l + 1] - d[l]) / (2.0 * e[l]);
                r = sqrt(g * g + 1.0);
                g = d[m] - d[l] + e[l] / (g + copysign(r, g));
                s = 1.0;
                c = 1.0;
                p = 0.0;
                for (i = m - 1; i >= l; i--) {
                    f = s * e[i];
                    b = c * e[i];
                    if (fabs(f) >= fabs(g)) {
                        c = g / f;
                        r = sqrt(c * c + 1.0);
                        e[i + 1] = f * r;
                        s = 1.0 / r;
                        c = c * s;
                    } else {
                        s = f / g;
                        r = sqrt(s * s + 1.0);
                        e[i + 1] = g * r;
                        c = 1.0 / r;
                        s = s * c;
                    }
                    g = d[i + 1] - p;
                    r = (d[i] - g) * s + 2.0 * c * b;
                    p = s * r;
                    d[i + 1] = g + p;
                    g = c * r - b;
                    for (k = 0; k < n; k++) {
                        f = z[k][i + 1];
                        z[k][i + 1] = s * z[k][i] + c * f;
                        z[k][i] = c * z[k][i] - s * f;
                    }
                }
                d[l] -= p;
                e[l] = g;
                e[m] = 0.0;
                goto label_1;
            }
        }
    }

    return;
}
