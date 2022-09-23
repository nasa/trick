/*
   PURPOSE: (Vector normalization)

   ASSUMPTIONS AND LIMITATIONS: ((3x1 vector implementation))

   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1990) (v1.0) (Init Release))) */

#include "trick/trick_math.h"

void dv_norm(double norm[3],    /* Out: Norm vector */
             double vect[3])
{                                      /* In: Vector to normalized */
    double square, magnitude;

    square = (vect[0] * vect[0] + vect[1] * vect[1] + vect[2] * vect[2]);
    if (square > 0.0) {
        magnitude = sqrt(square);
        norm[0] = vect[0] / magnitude;
        norm[1] = vect[1] / magnitude;
        norm[2] = vect[2] / magnitude;
    } else {
        norm[0] = 0.0;
        norm[1] = 0.0;
        norm[2] = 0.0;
    }

    return;
}
