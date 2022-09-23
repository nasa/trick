/*
   PURPOSE: (Matrix print)

   ASSUMPTIONS AND LIMITATIONS: ((3x3 Matrix implementation))

   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1990) (v1.0) (Init Release))) */

#include <stdio.h>
#include "trick/trick_math.h"

void dm_print(double mat[3][3])
{                                      /* In: matrix to be printed */
    int i, j;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            fprintf(stderr, " %f ", mat[i][j]);
        }
        fprintf(stderr, "\n");
    }
}
