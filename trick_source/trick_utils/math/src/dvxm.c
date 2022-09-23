/*
   PURPOSE: (Vector times matrix)
   ASSUMPTIONS AND LIMITATIONS: ((3x1 vector implementation))
   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1990) (v1.0) (Init Release)))
 */


#include "trick/trick_math.h"

void dvxm(double prod[],        /* Out: Product of the vector and the matrix */
          double vect[],        /* In: Vector */
          double mat[3][3])
{                                      /* In: Matrix */
    prod[0] = vect[0] * mat[0][0] + vect[1] * mat[1][0] + vect[2] * mat[2][0];
    prod[1] = vect[0] * mat[0][1] + vect[1] * mat[1][1] + vect[2] * mat[2][1];
    prod[2] = vect[0] * mat[0][2] + vect[1] * mat[1][2] + vect[2] * mat[2][2];

    return;
}
