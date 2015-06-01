/*
   PURPOSE: (Normalize a quaternion properly for all types of integration schemes.)

   REFERENCE: ((Flight Control - Ascent Flight Phase) (FSSR STS 83-0008A, Section 4.7.11 QUAT_NORM) (Volume 1, June 30,
   1985))

   ASSUMPTIONS AND LIMITATIONS: ((None))

   CLASS: (N/A)

   LIBRARY DEPENDENCY: ((quat_norm_integ.o))

   PROGRAMMERS: (((M Schira) (McDonnell Douglas) (Jan 1993) (v1.0) (Init Release)) ((Robert Gay) (Titan-LinCom) (June
   2001) (RPOC/CRV) (Removed rotation vector sign switch to work with all integration schemes, like rk4)))

 */

#include "trick/trick_math.h"


void quat_norm_integ(double q2[4],      /* Out: normalized quaternion */
                     double q1[4])
{                                      /* In: quaternion */

    double coef;

    coef = q1[0] * q1[0] + q1[1] * q1[1] + q1[2] * q1[2] + q1[3] * q1[3];

    if (fabs(1.0 - coef) < 0.00000023842) {
        coef = 2.0 / (1.0 + coef);
    } else {
        coef = 1.0 / sqrt(coef);
    }

    q2[0] = coef * q1[0];
    q2[1] = coef * q1[1];
    q2[2] = coef * q1[2];
    q2[3] = coef * q1[3];

    return;
}
