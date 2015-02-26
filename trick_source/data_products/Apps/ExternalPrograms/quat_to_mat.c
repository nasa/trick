#include "trick_utils/math/include/trick_math.h"

void quat_to_mat(
                          /* RETURN: -- Zero */
        double a[3][3],   /* OUT:    -- transformation matrix */
        double quat[4])   /* IN:     -- quaternion */
{

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
