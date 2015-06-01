/*
   PURPOSE: (Vector transfomation to skew symmetric form)
   ASSUMPTIONS AND LIMITATIONS: ((3x1 vector implementation))
   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1990) (v1.0) (Init Release)))
 */

#include "trick/trick_math.h"

void dv_skew(double skew[3][3], /* Out: Skew symmetic matrix */
             double vect[])
{                                      /* In: Vector to be transformed */
    skew[0][0] = 0.0;
    skew[1][1] = 0.0;
    skew[2][2] = 0.0;
    skew[0][1] = -vect[2];
    skew[0][2] = vect[1];
    skew[1][0] = vect[2];
    skew[1][2] = -vect[0];
    skew[2][0] = -vect[1];
    skew[2][1] = vect[0];

    return;
}
