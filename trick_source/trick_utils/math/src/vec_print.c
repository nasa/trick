/*
   PURPOSE: (Vector print)
   REFERENCE: ((See also: source file name))
   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1990) (v1.0) (Init Release)))
 */

#include <stdio.h>
#include "trick/trick_math.h"


void vec_print(double *vec,     /* In: Matrix to be printed */
               int n)
{                                      /* In: Array size */
    int i;

    for (i = 0; i < n; i++) {
        fprintf(stderr, " %16.12f ", vec[i]);
        fprintf(stderr, "\n");
    }

    return;
}
