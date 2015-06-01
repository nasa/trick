/*
   PURPOSE: (Matrix identity)

   ASSUMPTIONS AND LIMITATIONS: ((3x3 Matrix implementation))

   PROGRAMMERS: (((Les Quiocho) (NASA/Johnson Space Center) (Jan 1990) (v1.0) (Init Release))) */

#include "trick/trick_math.h"

void dm_ident(double mat[3][3])
{                                      /* Inout: Matrix identity */

    mat[0][0] = 1.0;
    mat[1][1] = 1.0;
    mat[2][2] = 1.0;
    mat[0][1] = 0.0;
    mat[1][0] = 0.0;
    mat[0][2] = 0.0;
    mat[2][0] = 0.0;
    mat[1][2] = 0.0;
    mat[2][1] = 0.0;

    return;
}
