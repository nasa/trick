/*
PURPOSE:
    (Matrix math in line code macros)

REFERENCE:
    (((Bailey, R.W)
     (Trick Simulation Environment Developer's Guide - Beta Release)
     (NASA:JSC #......)
     (JSC / Engineering Directorate / Automation and Robotics Division)
     (February 1991) ()))

PROGRAMMERS:
    (((Les Quiocho) (NASA/Johnson Space Center) (Jan 1991) (Initial Release))
     ((Les Quiocho) (NASA/JSC/ER3) (February 97) (Cleanup)))
     ((John M. Penn) (L3) (Aug 2010) (Documented for Doxygen)))
*/

/**
@page MATRIX_MACROS Matrix Macros

$TRICK_HOME/trick_source/trick_utils/math/include/matrix_macros.h

- Scalar parameters are expected to be of type float or double.
- Vector parameters are expected to be of type float[3] or double[3];
- Matrix parameters are expected to be of type float[3,3] or double[3,3];
*/

#ifndef _MATRIX_MACROS_H_
#define _MATRIX_MACROS_H_

#include <stdio.h>

/**
@page MATRIX_MACROS Matrix Macros
\b M_INIT( M )

Set all nine elements of the matrix m to 0.0.
\f[
M =
\begin{bmatrix}
 0.0 & 0.0 & 0.0 \\
 0.0 & 0.0 & 0.0 \\
 0.0 & 0.0 & 0.0
\end{bmatrix}
\f]
*/
#define M_INIT( mat ) { \
   mat[0][0] = mat[1][1] = mat[2][2] = 0.0 ; \
   mat[0][1] = mat[1][0] = 0.0 ; \
   mat[1][2] = mat[2][1] = 0.0 ; \
   mat[2][0] = mat[0][2] = 0.0 ; \
}

/**
@page MATRIX_MACROS Matrix Macros
\b M_IDENT( M )

Set M to the identity matrix.
\f[
M =
\begin{bmatrix}
 1.0 & 0.0 & 0.0 \\
 0.0 & 1.0 & 0.0 \\
 0.0 & 0.0 & 1.0
\end{bmatrix}
\f]
*/
#define M_IDENT( mat ) { \
   mat[0][0] = mat[1][1] = mat[2][2] = 1.0 ; \
   mat[0][1] = mat[1][0] = 0.0 ; \
   mat[1][2] = mat[2][1] = 0.0 ; \
   mat[2][0] = mat[0][2] = 0.0 ; \
}

/**
@page MATRIX_MACROS Matrix Macros
\b M_COPY(COPY, M)

Copy matrix M into matrix COPY.
\f[
copy_{i,j} = m_{i,j}:{i,j}\in 0..2
\f]
*/
#define M_COPY( copy , mat ) { \
   copy[0][0] = mat[0][0] ; copy[0][1] = mat[0][1] ; copy[0][2] = mat[0][2] ; \
   copy[1][0] = mat[1][0] ; copy[1][1] = mat[1][1] ; copy[1][2] = mat[1][2] ; \
   copy[2][0] = mat[2][0] ; copy[2][1] = mat[2][1] ; copy[2][2] = mat[2][2] ; \
}

/**
@page MATRIX_MACROS Matrix Macros
\b M_TRANS(TRANSPOSE ,M )

Transpose matrix M (swap rows and columns) and assign to matrix TRANSPOSE.
\f[
transpose_{i,j} = m_{j,i}:{i,j}\in 0..2
\f]
*/
#define M_TRANS( trans , mat ) { \
   trans[0][0]=mat[0][0] ; trans[0][1]=mat[1][0] ; trans[0][2]=mat[2][0] ; \
   trans[1][0]=mat[0][1] ; trans[1][1]=mat[1][1] ; trans[1][2]=mat[2][1] ; \
   trans[2][0]=mat[0][2] ; trans[2][1]=mat[1][2] ; trans[2][2]=mat[2][2] ; \
}

/**
@page MATRIX_MACROS Matrix Macros
\b M_ADD(S, A ,B)

Assign the sum of matrices A and B to the matrix S.
\f[
s_{i,j} = a_{i,j} + b_{i,j}: {i,j}\in 0..2
\f]
*/
#define M_ADD( sum , mat1 , mat2 ) { \
   sum[0][0] = mat1[0][0] + mat2[0][0] ; \
   sum[0][1] = mat1[0][1] + mat2[0][1] ; \
   sum[0][2] = mat1[0][2] + mat2[0][2] ; \
   sum[1][0] = mat1[1][0] + mat2[1][0] ; \
   sum[1][1] = mat1[1][1] + mat2[1][1] ; \
   sum[1][2] = mat1[1][2] + mat2[1][2] ; \
   sum[2][0] = mat1[2][0] + mat2[2][0] ; \
   sum[2][1] = mat1[2][1] + mat2[2][1] ; \
   sum[2][2] = mat1[2][2] + mat2[2][2] ; \
}

/**
@page MATRIX_MACROS Matrix Macros
\b M_SUB(D, A, B)

Subtract matrix B from matrix A and assign the difference the matrix D.
\f[
d_{i,j} = a_{i,j} - b_{i,j}: {i,j}\in 0..2
\f]
*/
#define M_SUB( diff , mat1 , mat2 ) { \
   diff[0][0] = mat1[0][0] - mat2[0][0] ; \
   diff[0][1] = mat1[0][1] - mat2[0][1] ; \
   diff[0][2] = mat1[0][2] - mat2[0][2] ; \
   diff[1][0] = mat1[1][0] - mat2[1][0] ; \
   diff[1][1] = mat1[1][1] - mat2[1][1] ; \
   diff[1][2] = mat1[1][2] - mat2[1][2] ; \
   diff[2][0] = mat1[2][0] - mat2[2][0] ; \
   diff[2][1] = mat1[2][1] - mat2[2][1] ; \
   diff[2][2] = mat1[2][2] - mat2[2][2] ; \
}

/**
@page MATRIX_MACROS Matrix Macros
\b MxV(P, M, V)

Assign the product of matrix M and vector V to vector P.
\f[
p_i = \sum_{j=0}^{2} m_{i,j} v_i: i\in 0..2
\f]
*/
#define MxV( prod , mat , vect ) { \
   prod[0] = mat[0][0] * vect[0] + mat[0][1] * vect[1] + mat[0][2] * vect[2] ; \
   prod[1] = mat[1][0] * vect[0] + mat[1][1] * vect[1] + mat[1][2] * vect[2] ; \
   prod[2] = mat[2][0] * vect[0] + mat[2][1] * vect[1] + mat[2][2] * vect[2] ; \
}

/**
@page MATRIX_MACROS Matrix Macros
\b MtxV(P, M, V)

Assign the product of the transpose of matrix M and vector V to vector P.
\f[
p_i = \sum_{j=0}^{2} m_{j,i} \cdot v_j: i\in 0..2
\f]
*/
#define MtxV( prod , mat , vect ) { \
   prod[0] = mat[0][0] * vect[0] + mat[1][0] * vect[1] + mat[2][0] * vect[2] ; \
   prod[1] = mat[0][1] * vect[0] + mat[1][1] * vect[1] + mat[2][1] * vect[2] ; \
   prod[2] = mat[0][2] * vect[0] + mat[1][2] * vect[1] + mat[2][2] * vect[2] ; \
}

/**
@page MATRIX_MACROS Matrix Macros
\b MxSCALAR(P, M, S)

Assign the product of the matrix M and scalar S to vector P.
\f[
p_i = m_{i,j} \cdot S: i\in 0..2
\f]
*/
#define MxSCALAR( prod , mat , scalar ) { \
   prod[0][0]=mat[0][0] * (scalar); \
   prod[0][1]=mat[0][1] * (scalar); \
   prod[0][2]=mat[0][2] * (scalar); \
   prod[1][0]=mat[1][0] * (scalar); \
   prod[1][1]=mat[1][1] * (scalar); \
   prod[1][2]=mat[1][2] * (scalar); \
   prod[2][0]=mat[2][0] * (scalar); \
   prod[2][1]=mat[2][1] * (scalar); \
   prod[2][2]=mat[2][2] * (scalar); \
}

/**
@page MATRIX_MACROS Matrix Macros
\b MxM(P, A, B)

Assign the product of the matrices A and B to matrix P.
\f[
p_{i,j} = \sum_{k=0}^{2}a_{i,k} \cdot b_{k,j}: i,j\in 0..2
\f]
*/
#define MxM( prod , mat1 , mat2 ) { \
 prod[0][0]=mat1[0][0]*mat2[0][0]+mat1[0][1]*mat2[1][0]+mat1[0][2]*mat2[2][0] ;\
 prod[0][1]=mat1[0][0]*mat2[0][1]+mat1[0][1]*mat2[1][1]+mat1[0][2]*mat2[2][1] ;\
 prod[0][2]=mat1[0][0]*mat2[0][2]+mat1[0][1]*mat2[1][2]+mat1[0][2]*mat2[2][2] ;\
 prod[1][0]=mat1[1][0]*mat2[0][0]+mat1[1][1]*mat2[1][0]+mat1[1][2]*mat2[2][0] ;\
 prod[1][1]=mat1[1][0]*mat2[0][1]+mat1[1][1]*mat2[1][1]+mat1[1][2]*mat2[2][1] ;\
 prod[1][2]=mat1[1][0]*mat2[0][2]+mat1[1][1]*mat2[1][2]+mat1[1][2]*mat2[2][2] ;\
 prod[2][0]=mat1[2][0]*mat2[0][0]+mat1[2][1]*mat2[1][0]+mat1[2][2]*mat2[2][0] ;\
 prod[2][1]=mat1[2][0]*mat2[0][1]+mat1[2][1]*mat2[1][1]+mat1[2][2]*mat2[2][1] ;\
 prod[2][2]=mat1[2][0]*mat2[0][2]+mat1[2][1]*mat2[1][2]+mat1[2][2]*mat2[2][2] ;\
}

/**
@page MATRIX_MACROS Matrix Macros
\b MtxM(P, A, B)

Assign the product of the A transpose and B to matrix P.
\f[
P = A^T \cdot B
\f]
\f[
p_{i,j} = \sum_{k=0}^{2}a_{k,i} \cdot b_{k,j}: i,j\in 0..2
\f]
*/
#define MtxM( prod , mat1 , mat2 ) { \
 prod[0][0]=mat1[0][0]*mat2[0][0]+mat1[1][0]*mat2[1][0]+mat1[2][0]*mat2[2][0] ;\
 prod[0][1]=mat1[0][0]*mat2[0][1]+mat1[1][0]*mat2[1][1]+mat1[2][0]*mat2[2][1] ;\
 prod[0][2]=mat1[0][0]*mat2[0][2]+mat1[1][0]*mat2[1][2]+mat1[2][0]*mat2[2][2] ;\
 prod[1][0]=mat1[0][1]*mat2[0][0]+mat1[1][1]*mat2[1][0]+mat1[2][1]*mat2[2][0] ;\
 prod[1][1]=mat1[0][1]*mat2[0][1]+mat1[1][1]*mat2[1][1]+mat1[2][1]*mat2[2][1] ;\
 prod[1][2]=mat1[0][1]*mat2[0][2]+mat1[1][1]*mat2[1][2]+mat1[2][1]*mat2[2][2] ;\
 prod[2][0]=mat1[0][2]*mat2[0][0]+mat1[1][2]*mat2[1][0]+mat1[2][2]*mat2[2][0] ;\
 prod[2][1]=mat1[0][2]*mat2[0][1]+mat1[1][2]*mat2[1][1]+mat1[2][2]*mat2[2][1] ;\
 prod[2][2]=mat1[0][2]*mat2[0][2]+mat1[1][2]*mat2[1][2]+mat1[2][2]*mat2[2][2] ;\
}

/**
@page MATRIX_MACROS Matrix Macros
\b MxMt(P, A, B)

Assign the product of the A and B transpose to matrix P.
\f[
P = A \cdot B^T
\f]
\f[
prod_{i,j} = \sum_{k=0}^{2}a_{i,k} \cdot b_{j,k}: i,j\in 0..2
\f]
*/
#define MxMt( prod , mat1 , mat2 ) { \
 prod[0][0]=mat1[0][0]*mat2[0][0]+mat1[0][1]*mat2[0][1]+mat1[0][2]*mat2[0][2] ;\
 prod[0][1]=mat1[0][0]*mat2[1][0]+mat1[0][1]*mat2[1][1]+mat1[0][2]*mat2[1][2] ;\
 prod[0][2]=mat1[0][0]*mat2[2][0]+mat1[0][1]*mat2[2][1]+mat1[0][2]*mat2[2][2] ;\
 prod[1][0]=mat1[1][0]*mat2[0][0]+mat1[1][1]*mat2[0][1]+mat1[1][2]*mat2[0][2] ;\
 prod[1][1]=mat1[1][0]*mat2[1][0]+mat1[1][1]*mat2[1][1]+mat1[1][2]*mat2[1][2] ;\
 prod[1][2]=mat1[1][0]*mat2[2][0]+mat1[1][1]*mat2[2][1]+mat1[1][2]*mat2[2][2] ;\
 prod[2][0]=mat1[2][0]*mat2[0][0]+mat1[2][1]*mat2[0][1]+mat1[2][2]*mat2[0][2] ;\
 prod[2][1]=mat1[2][0]*mat2[1][0]+mat1[2][1]*mat2[1][1]+mat1[2][2]*mat2[1][2] ;\
 prod[2][2]=mat1[2][0]*mat2[2][0]+mat1[2][1]*mat2[2][1]+mat1[2][2]*mat2[2][2] ;\
}

/**
@page MATRIX_MACROS Matrix Macros
\b MtxMt(P, A, B)

Assign the product of the A transpose and B transpose to matrix P.
\f[
P = A^T \cdot B^T
\f]
\f[
prod_{i,j} = \sum_{k=0}^{2}a_{k,i} \cdot b_{j,k}: i,j\in 0..2
\f]
*/
#define MtxMt( prod , mat1 , mat2 ) { \
 prod[0][0]=mat1[0][0]*mat2[0][0]+mat1[1][0]*mat2[0][1]+mat1[2][0]*mat2[0][2] ;\
 prod[0][1]=mat1[0][0]*mat2[1][0]+mat1[1][0]*mat2[1][1]+mat1[2][0]*mat2[1][2] ;\
 prod[0][2]=mat1[0][0]*mat2[2][0]+mat1[1][0]*mat2[2][1]+mat1[2][0]*mat2[2][2] ;\
 prod[1][0]=mat1[0][1]*mat2[0][0]+mat1[1][1]*mat2[0][1]+mat1[2][1]*mat2[0][2] ;\
 prod[1][1]=mat1[0][1]*mat2[1][0]+mat1[1][1]*mat2[1][1]+mat1[2][1]*mat2[1][2] ;\
 prod[1][2]=mat1[0][1]*mat2[2][0]+mat1[1][1]*mat2[2][1]+mat1[2][1]*mat2[2][2] ;\
 prod[2][0]=mat1[0][2]*mat2[0][0]+mat1[1][2]*mat2[0][1]+mat1[2][2]*mat2[0][2] ;\
 prod[2][1]=mat1[0][2]*mat2[1][0]+mat1[1][2]*mat2[1][1]+mat1[2][2]*mat2[1][2] ;\
 prod[2][2]=mat1[0][2]*mat2[2][0]+mat1[1][2]*mat2[2][1]+mat1[2][2]*mat2[2][2] ;\
}

/**
@page MATRIX_MACROS Matrix Macros
\b M_PRINT(M)

Print matrix M to stderr.
*/
#define M_PRINT( mat ) { \
   fprintf( stderr, "\n%f %f %f\n" , mat[0][0] , mat[0][1] , mat[0][2] ) ; \
   fprintf( stderr, "%f %f %f\n" , mat[1][0] , mat[1][1] , mat[1][2] ) ; \
   fprintf( stderr, "%f %f %f\n" , mat[2][0] , mat[2][1] , mat[2][2] ) ; \
}

/**
@page MATRIX_MACROS Matrix Macros
\b MxMxV(P, A, B, V)
Assigns the product of A, B, and V to P.
\f[
p_i = \sum_{j=0}^{2} A_{i,j} \left( \sum_{k=0}^{2} B_{j,k} \cdot v_i \right) : i\in 0..2
\f]
*/
#define MxMxV(prod, mat1, mat2, vect ) { \
  prod[0] = mat1[0][0] *(mat2[0][0] * vect[0] + mat2[0][1] * vect[1] + mat2[0][2] * vect[2])+\
            mat1[0][1] *(mat2[1][0] * vect[0] + mat2[1][1] * vect[1] + mat2[1][2] * vect[2])+\
            mat1[0][2] *(mat2[2][0] * vect[0] + mat2[2][1] * vect[1] + mat2[2][2] * vect[2]);\
  prod[1] = mat1[1][0] *(mat2[0][0] * vect[0] + mat2[0][1] * vect[1] + mat2[0][2] * vect[2])+\
            mat1[1][1] *(mat2[1][0] * vect[0] + mat2[1][1] * vect[1] + mat2[1][2] * vect[2])+\
            mat1[1][2] *(mat2[2][0] * vect[0] + mat2[2][1] * vect[1] + mat2[2][2] * vect[2]);\
  prod[2] = mat1[2][0] *(mat2[0][0] * vect[0] + mat2[0][1] * vect[1] + mat2[0][2] * vect[2])+\
            mat1[2][1] *(mat2[1][0] * vect[0] + mat2[1][1] * vect[1] + mat2[1][2] * vect[2])+\
            mat1[2][2] *(mat2[2][0] * vect[0] + mat2[2][1] * vect[1] + mat2[2][2] * vect[2]);\
}
#endif
