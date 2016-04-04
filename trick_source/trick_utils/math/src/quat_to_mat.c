/*
   PURPOSE: (Compute the orthonormal transformation matrix from the corresponding normalized quaternion)

   PROGRAMMERS: (((M Schira) (McDonnell Douglas) (Jan 1993) (v1.0) (Init Release)))

 */

#include "trick/trick_math.h"

void quat_to_mat(double a[3][3],       /* Out: transformation matrix */
                 double quat[4])
{                                      /* In: left handed quaternion */

    a[0][0] = 1. - 2. * (quat[2] * quat[2] + quat[3] * quat[3]);
    a[1][1] = 1. - 2. * (quat[3] * quat[3] + quat[1] * quat[1]);
    a[2][2] = 1. - 2. * (quat[1] * quat[1] + quat[2] * quat[2]);
    a[1][0] = 2. * (quat[1] * quat[2] + quat[0] * quat[3]);
    a[0][1] = 2. * (quat[1] * quat[2] - quat[0] * quat[3]);
    a[2][0] = 2. * (quat[1] * quat[3] - quat[0] * quat[2]);
    a[0][2] = 2. * (quat[1] * quat[3] + quat[0] * quat[2]);
    a[2][1] = 2. * (quat[2] * quat[3] + quat[0] * quat[1]);
    a[1][2] = 2. * (quat[2] * quat[3] - quat[0] * quat[1]);

    return;
}
