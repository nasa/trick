/*
   PURPOSE: (Vector cross product) ASSUMPTIONS AND LIMITATIONS: ((3x1 vector implementation))

   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1990) (v1.0) (Init Release)))
 */

#include "trick/trick_math.h"

void dv_cross(double cross[],   /* Out: cross product of the two vectors */
              double vect1[],   /* In: vector 1 */
              double vect2[])
{                                      /* In: vector 2 */
    cross[0] = ((vect1[1] * vect2[2]) - (vect1[2] * vect2[1]));
    cross[1] = ((vect1[2] * vect2[0]) - (vect1[0] * vect2[2]));
    cross[2] = ((vect1[0] * vect2[1]) - (vect1[1] * vect2[0]));

    return;
}
