/*******************************************************************************
*                                                                              *
* Trick Simulation Environment Software                                        *
*                                                                              *
* Copyright (c) 1996,1997 LinCom Corporation, Houston, TX                      *
* All rights reserved.                                                         *
*                                                                              *
* Copyrighted by LinCom Corporation and proprietary to it. Any unauthorized    *
* use of Trick Software including source code, object code or executables is   *
* strictly prohibited and LinCom assumes no liability for such actions or      *
* results thereof.                                                             *
*                                                                              *
* Trick Software has been developed under NASA Government Contracts and        *
* access to it may be granted for Government work by the following contact:    *
*                                                                              *
* Contact: Charles Gott, Branch Chief                                          *
*          Simulation and Graphics Branch                                      *
*          Automation, Robotics, & Simulation Division                         *
*          NASA, Johnson Space Center, Houston, TX                             *
*                                                                              *
*******************************************************************************/
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

#ifndef _COMPLEX_H_
#define _COMPLEX_H_

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
