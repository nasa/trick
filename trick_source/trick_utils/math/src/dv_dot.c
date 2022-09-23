/*
   PURPOSE: (Vector dot product) ASSUMPTIONS AND LIMITATIONS: ((3x1 vector implementation))

   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1990) (v1.0) (Init Release))) */

#include "trick/trick_math.h"

double dv_dot(                  /* Return: Scalar dot or inner product */
                 double vect1[],        /* In: Vector 1 */
                 double vect2[])
{                                      /* In: Vector 2 */
    return (vect1[0] * vect2[0] + vect1[1] * vect2[1] + vect1[2] * vect2[2]);
}
