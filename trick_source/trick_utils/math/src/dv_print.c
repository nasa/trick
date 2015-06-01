/*
   PURPOSE: (Vector print)
   ASSUMPTIONS AND LIMITATIONS: ((3x1 vector implementation))
   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1990) (v1.0) (Init Release))) */

#include <stdio.h>
#include "trick/trick_math.h"

void dv_print(double vect[])
{                                      /* In: Vector to be printed */
    int j;
    for (j = 0; j < 3; j++) {
        fprintf(stderr, " %f ", vect[j]);
    }
}
