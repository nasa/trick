/*
   PURPOSE: (Matrix transpose times vector)

   ASSUMPTIONS AND LIMITATIONS: ((3x3 Matrix implementation))

   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1990) (v1.0) (Init Release))) */

#include "trick/trick_math.h"

void dmtxv(double prod[],       /* Out: Product of the matrix and vector */
           double mat[3][3],    /* In: Matrix */
           double vect[])
{                                      /* In: Vector */
    prod[0] = mat[0][0] * vect[0] + mat[1][0] * vect[1] + mat[2][0] * vect[2];
    prod[1] = mat[0][1] * vect[0] + mat[1][1] * vect[1] + mat[2][1] * vect[2];
    prod[2] = mat[0][2] * vect[0] + mat[1][2] * vect[1] + mat[2][2] * vect[2];

    return;
}
