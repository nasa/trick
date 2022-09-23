/*
PURPOSE:
       (Calculate the magnitude of a 3-D vector.)

REFERENCE:
       ((None))

ASSUMPTIONS AND LIMITATIONS:
       ((Euler sequence is input interactively by user.))

PROGRAMMERS:
       (((Robert W. Bailey) (LinCom Corp) (4/9/91)
         (Trick-CR-00004) (Gravity / Inertial Frame Models)))
         (Dorci L. Smith) (TSC-AEU) (May 2002) (Reduce ext program size))
*/

#include <stdio.h>
#include <float.h>
#include "trick_utils/math/include/vector_macros.h"

#define X 0
#define Y 1
#define Z 2

int extGetNextRecord(double *in, int numIn, double *out, int numOut)
{

        /* vector magnitude : mag(vect) = sqrt(vect * vect) */
        out[0] = V_MAG(in);

        return (0);
}
