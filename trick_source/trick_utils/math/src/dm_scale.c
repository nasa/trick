/*
   PURPOSE: (Matrix scaling)

   REFERENCE: ((See also: source file name))

   ASSUMPTIONS AND LIMITATIONS: ((3x3 Matrix implementation))

   PROGRAMMERS: (((Les Quiocho) (NASA/Johnson Space Center) (Jan 1990) (v1.0) (Init Release))) */

#include "trick/trick_math.h"

void dm_scale(double prod[3][3],        /* Out: Product of the two matrices */
              double mat[3][3], /* In: Matrix */
              double scalar)
{                                      /* In: Scalar multiplier */
    prod[0][0] = mat[0][0] * scalar;
    prod[0][1] = mat[0][1] * scalar;
    prod[0][2] = mat[0][2] * scalar;

    prod[1][0] = mat[1][0] * scalar;
    prod[1][1] = mat[1][1] * scalar;
    prod[1][2] = mat[1][2] * scalar;

    prod[2][0] = mat[2][0] * scalar;
    prod[2][1] = mat[2][1] * scalar;
    prod[2][2] = mat[2][2] * scalar;

    return;
}
