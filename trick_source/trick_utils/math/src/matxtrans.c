/*
  PURPOSE: (Matrix times Matrix transpose)
  ASSUMPTIONS AND LIMITATIONS: ((Square matrix))
  PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1993) (v1.0) (Init Release))
                ((Robert McPhail) (CACI) (Feb 2017) (Updated algorithm))) */

#include "trick/trick_math.h"

void matxtrans(double **prod,   /* Out: product of the two matrices */
               double **mat1,   /* In: matrix 1 */
               double **mat2,   /* In: matrix 2 */
               int n) {         /* In: array size */

   int i, j, k;
   double temp;

   for (i = 0; i < n; i++) {
       for (j = 0; j < n; j++) {
           temp = 0.0;
           for (k = 0; k < n; k++) {
               temp += mat1[i][k] * mat2[j][k];
           }
           prod[i][j] = temp;
       }
   }
   return;
}
