/*
   PURPOSE: (Multiply 2 quaternions, q3 = q1*q2)

   REFERENCE: ((Flight Control - Ascent Flight Phase) (FSSR STS 83-0008A, Section 4.7.10 QUAT_MULT) (Volume 1, June 30,
   1985))

   PROGRAMMERS: (((M Schira) (McDonnell Douglas) (Jan 1993) (v1.0) (Init Release)))

 */

#include "trick/trick_math.h"

void quat_mult(double q3[4],    /* Out: quaternion */
               double q1[4],    /* In: quaternion */
               double q2[4])
{                                      /* In: quaternion */
    q3[0] = q1[0] * q2[0] - (q1[1] * q2[1] + q1[2] * q2[2] + q1[3] * q2[3]);
    q3[1] = q1[0] * q2[1] + q2[0] * q1[1] + q1[2] * q2[3] - q1[3] * q2[2];
    q3[2] = q1[0] * q2[2] + q2[0] * q1[2] + q1[3] * q2[1] - q1[1] * q2[3];
    q3[3] = q1[0] * q2[3] + q2[0] * q1[3] + q1[1] * q2[2] - q1[2] * q2[1];

    return;
}
