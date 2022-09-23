/*
   PURPOSE: (Vector scaling)
   ASSUMPTIONS AND LIMITATIONS: ((3x1 vector implementation))
   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1990) (v1.0) (Init Release)))
 */

#include "trick/trick_math.h"

void dv_scale(double prod[],    /* Out: Produce of the vector and the scalar */
              double vect[],    /* In: Vector */
              double scalar)
{                                      /* In: Scalar multiplier */
    prod[0] = vect[0] * scalar;
    prod[1] = vect[1] * scalar;
    prod[2] = vect[2] * scalar;

    return;
}
