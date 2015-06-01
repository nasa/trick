/*
   PURPOSE: (Vector subtraction)
   ASSUMPTIONS AND LIMITATIONS: ((3x1 vector implementation))
   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1990) (v1.0) (Init Release)))
 */

#include "trick/trick_math.h"

void dv_sub(double diff[],      /* Out: Difference of the two vectors */
            double vect1[],     /* In: Vector 1 */
            double vect2[])
{                                      /* In: Vector 2 */
    diff[0] = vect1[0] - vect2[0];
    diff[1] = vect1[1] - vect2[1];
    diff[2] = vect1[2] - vect2[2];

    return;
}
