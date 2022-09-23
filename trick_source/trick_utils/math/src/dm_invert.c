/*
   PURPOSE: (Invert a 3X3 matrix.)

   ASSUMPTIONS AND LIMITATIONS: ((Determinant is non-zero))

   PROGRAMMERS: (((Robert W. Bailey) (LinCom Corporation) (4/1/91) (Trick-CR-00004) (Gravity / Inertial Frame Models))) */

#include "trick/trick_math.h"

int dm_invert(double inv[3][3], /* Out: The 3X3 inverse of matrix 'm' */
              double m[3][3])
{                                      /* In: A 3X3 matrix */
    double determinant;         /* The determinant of the input 'm' matrix */

    /* Calculate the determinant and test it */
    /* (Save the intermediate calculations in 'inv' for later use) */
    inv[0][0] = m[1][1] * m[2][2] - m[2][1] * m[1][2];
    inv[0][1] = m[2][1] * m[0][2] - m[0][1] * m[2][2];
    inv[0][2] = m[0][1] * m[1][2] - m[1][1] * m[0][2];

    determinant = m[0][0] * inv[0][0] + m[1][0] * inv[0][1] + m[2][0] * inv[0][2];
    if (determinant == 0.0) {
        fprintf(stderr, "trick_utils/math/dm_invert.c: 3x3 matrix has zero determinant\n");
        return (TM_ZERO_DET);
    }

    /* Calculate the inverse matrix */
    inv[0][0] /= determinant;
    inv[0][1] /= determinant;
    inv[0][2] /= determinant;
    inv[1][0] = (m[2][0] * m[1][2] - m[1][0] * m[2][2]) / determinant;
    inv[1][1] = (m[0][0] * m[2][2] - m[2][0] * m[0][2]) / determinant;
    inv[1][2] = (m[1][0] * m[0][2] - m[0][0] * m[1][2]) / determinant;
    inv[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) / determinant;
    inv[2][1] = (m[2][0] * m[0][1] - m[0][0] * m[2][1]) / determinant;
    inv[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) / determinant;

    return (TM_SUCCESS);
}
