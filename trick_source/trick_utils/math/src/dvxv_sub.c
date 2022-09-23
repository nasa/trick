/*
   PURPOSE: (Vector cross product difference)
   ASSUMPTIONS AND LIMITATIONS: ((3x1 vector implementation))
   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1990) (v1.0) (Init Release)))
 */

#include "trick/trick_math.h"

void dvxv_sub(double diff[],    /* Out: Difference of cross product and difference */
              double vect1[],   /* In: Vector1 */
              double vect2[])
{                                      /* In: Vector2 */
    diff[0] -= ((vect1[1] * vect2[2]) - (vect1[2] * vect2[1]));
    diff[1] -= ((vect1[2] * vect2[0]) - (vect1[0] * vect2[2]));
    diff[2] -= ((vect1[0] * vect2[1]) - (vect1[1] * vect2[0]));

    return;
}
