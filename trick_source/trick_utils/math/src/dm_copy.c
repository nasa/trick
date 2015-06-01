/*
   PURPOSE: (Matrix copy)

   REFERENCE: ((See also: source file name))

   ASSUMPTIONS AND LIMITATIONS: ((3x3 Matrix implementation))

   PROGRAMMERS: (((Les Quiocho) (NASA/Johnson Space Center) (Jan 1990) (v1.0) (Init Release))) */

#include "trick/trick_math.h"

void dm_copy(double copy[3][3], /* Out: copy of the matrix */
             double mat[3][3])
{                                      /* In: matrix */

    copy[0][0] = mat[0][0];
    copy[0][1] = mat[0][1];
    copy[0][2] = mat[0][2];
    copy[1][0] = mat[1][0];
    copy[1][1] = mat[1][1];
    copy[1][2] = mat[1][2];
    copy[2][0] = mat[2][0];
    copy[2][1] = mat[2][1];
    copy[2][2] = mat[2][2];

    return;
}
