/*
   PURPOSE: (Vector copy)

   ASSUMPTIONS AND LIMITATIONS: ((3x1 vector implementation))

   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1990) (v1.0) (Init Release))) */

#include "trick/trick_math.h"

void dv_copy(double copy[],     /* Out: copy of the vector */
             double vect[])
{                                      /* In: vector to be copied */

    copy[0] = vect[0];
    copy[1] = vect[1];
    copy[2] = vect[2];

    return;
}
