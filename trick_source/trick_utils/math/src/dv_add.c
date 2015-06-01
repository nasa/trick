/*
   PURPOSE: (Vector addition)

   ASSUMPTIONS AND LIMITATIONS: ((3x1 vector implementation))

   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1990) (v1.0) (Init Release))) */

#include "trick/trick_math.h"

void dv_add(double sum[],       /* Out: Sum of the two vectors */
            double vect1[],     /* In: Vector 1 */
            double vect2[])
{                                      /* In: Vector 2 */
    sum[0] = vect1[0] + vect2[0];
    sum[1] = vect1[1] + vect2[1];
    sum[2] = vect1[2] + vect2[2];

    return;
}
