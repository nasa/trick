Trick includes some matrix and vector math functions as well as some linear algebra routines. Many of Trick's routines are limited to 3x3 matricies.  The GNU Scientific Library includes more generic and faster implementations of these functions.  Here are some examples on how to use the GSL in cases where our provided functions are not adequate.

### dm_add: matrix addition
```c
// C = A + B

#include <gsl/gsl_blas.h>

double A[3][3] ;
double B[3][3] ;
double C[3][3] ;

// copies contents of A into C
cblas_dcopy(3*3,(double *)A,1,(double *)C,1) ;
// computes (1.0 * B), then adds to C
cblas_daxpy(3*3,1.0,(double *)B,1,(double *)C,1) ;
```