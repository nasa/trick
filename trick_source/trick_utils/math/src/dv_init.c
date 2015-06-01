/*
   PURPOSE: (Vector initialization)

   ASSUMPTIONS AND LIMITATIONS: ((3x1 vector implementation))

   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1990) (v1.0) (Init Release))) */

#include "trick/trick_math.h"

void dv_init(double vect[])
{                                      /* Inout: Vector to be initialized */

    vect[0] = 0.0;
    vect[1] = 0.0;
    vect[2] = 0.0;

    return;
}
