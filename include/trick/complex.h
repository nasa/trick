/*
PURPOSE:
    (FORTRAN Complex variable type definition)
REFERENCE:
    ((none))
ASSUMPTIONS AND LIMITATIONS:
    ((real part is first imaginary part is second))
PROGRAMMERS:
    (((Robert W. Bailey) (LinCom) (7/96)))
*/

#ifndef COMPLEX_H
#define COMPLEX_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {

    float real;       /* --  Real part */
    float imaginary;  /* --  Imaginary part */

} COMPLEX;

typedef struct {

    double real;      /* --  Real part */
    double imaginary; /* --  Imaginary part */

} DCOMPLEX;

#ifdef __cplusplus
}
#endif
#endif
