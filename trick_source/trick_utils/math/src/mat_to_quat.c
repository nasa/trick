/*
   PURPOSE: (Compute the normalized left handed quaternion from the corresponding orthonormal transformation matrix)

   REFERENCE: ((Flight Control - Orbit DAP) (FSSR STS 83-0009C OI-20, Section 4.3.8 MAT_TO_QUAT) (November 29, 1990))

   PROGRAMMERS: (((M Schira) (McDonnell Douglas) (Jan 1993) (v1.0) (Init Release)))

 */

#include "trick/trick_math.h"

void mat_to_quat(double quat[4],        /* Out: Left handed quaternion */
                 double a[3][3])
{                                      /* In: Transformation matrix */

    double qv[3];
    double T, temp, q0;
    int i, j, k, k1, l, n, jki;

    i = 0;
    T = a[0][0] + a[1][1] + a[2][2];
    q0 = T;

    for (k = 1; k < 4; k++) {
        k1 = k - 1;
        if (a[k1][k1] > q0) {
            i = k;
            q0 = a[k1][k1];
        }
    }

    temp = 1.0 + 2.0 * q0 - T;
    T = sqrt(temp);

    for (n = 1; n < 4; n++) {
        k = (n % 3) + 1;
        j = 6 - n - k;
        l = i * (n - i);
        if (l == 0) {
            qv[n - 1] = (a[j - 1][k - 1] - a[k - 1][j - 1]) / T;
            q0 = qv[n - 1];
        } else {
            jki = j + k - i;
            qv[jki - 1] = (a[j - 1][k - 1] + a[k - 1][j - 1]) / T;
        }
    }

    if (i == 0)
        q0 = T;
    else
        qv[i - 1] = T;

    T = copysign(0.5, q0);

    quat[0] = T * q0;

    quat[1] = T * qv[0];
    quat[2] = T * qv[1];
    quat[3] = T * qv[2];

    return;
}
