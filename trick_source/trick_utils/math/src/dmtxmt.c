/*
   PURPOSE: (Matrix transpose times matrix transpose)

   ASSUMPTIONS AND LIMITATIONS: ((3x3 Matrix implementation))

   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1990) (v1.0) (Init Release))) */

#include "trick/trick_math.h"

void dmtxmt(                    /* Return: -- none */
               double prod[3][3],       /* Out: Product of the two matrices */
               double mat1[3][3],       /* In: Matrix 1 */
               double mat2[3][3])
{                                      /* In: Matrix 2 */
    double trans1[3][3], trans2[3][3];

    dm_trans(trans1, mat1);
    dm_trans(trans2, mat2);

    prod[0][0] = trans1[0][0] * trans2[0][0] + trans1[0][1] * trans2[1][0] + trans1[0][2] * trans2[2][0];
    prod[0][1] = trans1[0][0] * trans2[0][1] + trans1[0][1] * trans2[1][1] + trans1[0][2] * trans2[2][1];
    prod[0][2] = trans1[0][0] * trans2[0][2] + trans1[0][1] * trans2[1][2] + trans1[0][2] * trans2[2][2];

    prod[1][0] = trans1[1][0] * trans2[0][0] + trans1[1][1] * trans2[1][0] + trans1[1][2] * trans2[2][0];
    prod[1][1] = trans1[1][0] * trans2[0][1] + trans1[1][1] * trans2[1][1] + trans1[1][2] * trans2[2][1];
    prod[1][2] = trans1[1][0] * trans2[0][2] + trans1[1][1] * trans2[1][2] + trans1[1][2] * trans2[2][2];

    prod[2][0] = trans1[2][0] * trans2[0][0] + trans1[2][1] * trans2[1][0] + trans1[2][2] * trans2[2][0];
    prod[2][1] = trans1[2][0] * trans2[0][1] + trans1[2][1] * trans2[1][1] + trans1[2][2] * trans2[2][1];
    prod[2][2] = trans1[2][0] * trans2[0][2] + trans1[2][1] * trans2[1][2] + trans1[2][2] * trans2[2][2];

    return;
}
