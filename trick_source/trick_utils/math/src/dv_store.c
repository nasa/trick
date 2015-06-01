/*
   PURPOSE: (Vector store with a scalar)
   ASSUMPTIONS AND LIMITATIONS: ((3x1 vector implementation))
   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1990) (v1.0) (Init Release)))
 */

#include "trick/trick_math.h"

void dv_store(double vect[],    /* Out: Vector */
              double scalar)
{                                      /* In: Scalar to be stored */
    vect[0] = scalar;
    vect[1] = scalar;
    vect[2] = scalar;

    return;
}
