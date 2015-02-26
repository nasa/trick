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
   PURPOSE: (Matrix transpose)

   ASSUMPTIONS AND LIMITATIONS: ((3x3 Matrix implementation))

   PROGRAMMERS: (((Les Quiocho) (NASA/Johnson Space Center) (Jan 1990) (v1.0) (Init Release))) */

/* 
 * $Id: dm_trans.c 49 2009-02-02 22:37:59Z lin $
 */

#include "../include/trick_math.h"

void dm_trans(double trans[3][3],       /* Out: transpose of the matrix */
              double mat[3][3])
{                                      /* In: matrix to be transposed */
    trans[0][0] = mat[0][0];
    trans[0][1] = mat[1][0];
    trans[0][2] = mat[2][0];
    trans[1][0] = mat[0][1];
    trans[1][1] = mat[1][1];
    trans[1][2] = mat[2][1];
    trans[2][0] = mat[0][2];
    trans[2][1] = mat[1][2];
    trans[2][2] = mat[2][2];

    return;
}
