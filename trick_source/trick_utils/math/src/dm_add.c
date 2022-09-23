/*
   PURPOSE: (Matrix addition)

   ASSUMPTIONS AND LIMITATIONS: ((3x3 Matrix implementation))

   PROGRAMMERS: (((Les Quiocho) (NASA/Johnson Space Center) (Jan 1990) (v1.0) (Init Release))) */

#include "trick/trick_math.h"

void dm_add(double sum[3][3],   /* Out: Sum of the two matrices */
            double mat1[3][3],  /* In: Matrix 1 */
            double mat2[3][3])
{                                      /* In: Matrix 2 */

    sum[0][0] = mat1[0][0] + mat2[0][0];
    sum[0][1] = mat1[0][1] + mat2[0][1];
    sum[0][2] = mat1[0][2] + mat2[0][2];

    sum[1][0] = mat1[1][0] + mat2[1][0];
    sum[1][1] = mat1[1][1] + mat2[1][1];
    sum[1][2] = mat1[1][2] + mat2[1][2];

    sum[2][0] = mat1[2][0] + mat2[2][0];
    sum[2][1] = mat1[2][1] + mat2[2][1];
    sum[2][2] = mat1[2][2] + mat2[2][2];

    return;
}
