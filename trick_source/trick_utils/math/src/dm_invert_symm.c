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
   PURPOSE: (Invert a symmetric 3X3 matrix.)

   ASSUMPTIONS AND LIMITATIONS: ((Matrix is symmetric) (Determinate is non-zero))

   PROGRAMMERS: (((Robert W. Bailey) (LinCom Corporation) (4/1/91) (Trick-CR-00004) (Gravity / Inertial Frame Models))) */

/* 
 * $Id: dm_invert_symm.c 49 2009-02-02 22:37:59Z lin $
 */

#include "../include/trick_math.h"

int dm_invert_symm(double inv[3][3],    /* Out: The 3X3 inverse of matrix 'm' */
                   double m[3][3])
{                                      /* In: A 3X3 matrix */

    double determinate;         /* The determinate of 'm' */

    /* Calculate the determinate and test it */
    inv[0][0] = m[1][1] * m[2][2] - m[2][1] * m[1][2];
    inv[0][1] = m[2][1] * m[0][2] - m[0][1] * m[2][2];
    inv[0][2] = m[0][1] * m[1][2] - m[1][1] * m[0][2];

    determinate = m[0][0] * inv[0][0] + m[1][0] * inv[0][1] + m[2][0] * inv[0][2];

    if (determinate == 0.0) {
        fprintf(stderr, "trick_utils/math/dm_invert_symm.c: Tried to invert " "a 3x3 matrix with a zero determinate.");
        return (TM_ZERO_DET);
    }

    /* Calculate the inverse matrix, and return '1' */
    inv[0][0] /= determinate;
    inv[0][1] /= determinate;
    inv[0][2] /= determinate;
    inv[1][0] = inv[0][1];
    inv[1][1] = (m[0][0] * m[2][2] - m[2][0] * m[0][2]) / determinate;
    inv[1][2] = (m[1][0] * m[0][2] - m[0][0] * m[1][2]) / determinate;
    inv[2][0] = inv[0][2];
    inv[2][1] = inv[1][2];
    inv[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) / determinate;

    return (TM_SUCCESS);
}
