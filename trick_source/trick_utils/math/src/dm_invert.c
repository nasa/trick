/*
   PURPOSE: (Invert a 3X3 matrix.)

   ASSUMPTIONS AND LIMITATIONS: ((Determinate is non-zero))

   PROGRAMMERS: (((Robert W. Bailey) (LinCom Corporation) (4/1/91) (Trick-CR-00004) (Gravity / Inertial Frame Models))) */

#include "trick/trick_math.h"

int dm_invert(double inv[3][3], /* Out: The 3X3 inverse of matrix 'm' */
              double m[3][3])
{                                      /* In: A 3X3 matrix */
    double determinate;         /* The determinate of the input 'm' matrix */

    /* Calculate the determinate and test it */
    /* (Save the intermediate calculations in 'inv' for later use) */
    inv[0][0] = m[1][1] * m[2][2] - m[2][1] * m[1][2];
    inv[0][1] = m[2][1] * m[0][2] - m[0][1] * m[2][2];
    inv[0][2] = m[0][1] * m[1][2] - m[1][1] * m[0][2];

    determinate = m[0][0] * inv[0][0] + m[1][0] * inv[0][1] + m[2][0] * inv[0][2];
    if (determinate == 0.0) {
        fprintf(stderr, "trick_utils/math/dm_invert.c: 3x3 matrix has zero determinate\n");
        return (TM_ZERO_DET);
    }

    /* Calculate the inverse matrix */
    inv[0][0] /= determinate;
    inv[0][1] /= determinate;
    inv[0][2] /= determinate;
    inv[1][0] = (m[2][0] * m[1][2] - m[1][0] * m[2][2]) / determinate;
    inv[1][1] = (m[0][0] * m[2][2] - m[2][0] * m[0][2]) / determinate;
    inv[1][2] = (m[1][0] * m[0][2] - m[0][0] * m[1][2]) / determinate;
    inv[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) / determinate;
    inv[2][1] = (m[2][0] * m[0][1] - m[0][0] * m[2][1]) / determinate;
    inv[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) / determinate;

    return (TM_SUCCESS);
}
