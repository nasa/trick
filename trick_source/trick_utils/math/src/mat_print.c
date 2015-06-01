/*
   PURPOSE: (Matrix print)
   ASSUMPTIONS AND LIMITATIONS: ((Square Matrix))
   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1993) (v1.0) (Init Release))) */

#include <stdio.h>
#include "trick/trick_math.h"

void mat_print(double **mat,    /* In: Matrix to be printed */
               int n)
{                                      /* In: Array size */
    int i, j;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            fprintf(stderr, " %16.12f ", mat[i][j]);
        }
        fprintf(stderr, "\n");
    }

    return;
}
