/*
   PURPOSE: (Vector magnitude) ASSUMPTIONS AND LIMITATIONS: ((3x1 vector implementation))

   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1990) (v1.0) (Init Release))) */

#include "trick/trick_math.h"

double dv_mag(                  /* Return: Magnitude of the vector */
                 double vect[])
{                                      /* In: Vector */
    return (sqrt(vect[0] * vect[0] + vect[1] * vect[1] + vect[2] * vect[2]));
}
