/*
   PURPOSE: (Matrix transpose)

   ASSUMPTIONS AND LIMITATIONS: ((3x3 Matrix implementation))

   PROGRAMMERS: (((Les Quiocho) (NASA/Johnson Space Center) (Jan 1990) (v1.0) (Init Release))) */

#include "trick/trick_math.h"

void dm_trans(double trans[3][3],       /* Out: transpose of the matrix */
              double mat[3][3])
{                                      /* In: matrix to be transposed */
    trans[0][0] = mat[0][0];
    trans[0][1] = mat[1][0];
    trans[0][2] = mat[2][0];
    trans[1][0] = mat[0][1];
    trans[1][1] = mat[1][1];
    trans[1][2] = mat[2][1];
    trans[2][0] = mat[0][2];
    trans[2][1] = mat[1][2];
    trans[2][2] = mat[2][2];

    return;
}
