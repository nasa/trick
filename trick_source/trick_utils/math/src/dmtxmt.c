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
   PURPOSE: (Matrix transpose times matrix transpose)

   ASSUMPTIONS AND LIMITATIONS: ((3x3 Matrix implementation))

   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (Jan 1990) (v1.0) (Init Release))) */

/* 
 * $Id: dmtxmt.c 49 2009-02-02 22:37:59Z lin $
 */

#include "../include/trick_math.h"

void dmtxmt(                    /* Return: -- none */
               double prod[3][3],       /* Out: Product of the two matrices */
               double mat1[3][3],       /* In: Matrix 1 */
               double mat2[3][3])
{                                      /* In: Matrix 2 */
    double trans1[3][3], trans2[3][3];

    dm_trans(trans1, mat1);
    dm_trans(trans2, mat2);

    prod[0][0] = trans1[0][0] * trans2[0][0] + trans1[0][1] * trans2[1][0] + trans1[0][2] * trans2[2][0];
    prod[0][1] = trans1[0][0] * trans2[0][1] + trans1[0][1] * trans2[1][1] + trans1[0][2] * trans2[2][1];
    prod[0][2] = trans1[0][0] * trans2[0][2] + trans1[0][1] * trans2[1][2] + trans1[0][2] * trans2[2][2];

    prod[1][0] = trans1[1][0] * trans2[0][0] + trans1[1][1] * trans2[1][0] + trans1[1][2] * trans2[2][0];
    prod[1][1] = trans1[1][0] * trans2[0][1] + trans1[1][1] * trans2[1][1] + trans1[1][2] * trans2[2][1];
    prod[1][2] = trans1[1][0] * trans2[0][2] + trans1[1][1] * trans2[1][2] + trans1[1][2] * trans2[2][2];

    prod[2][0] = trans1[2][0] * trans2[0][0] + trans1[2][1] * trans2[1][0] + trans1[2][2] * trans2[2][0];
    prod[2][1] = trans1[2][0] * trans2[0][1] + trans1[2][1] * trans2[1][1] + trans1[2][2] * trans2[2][1];
    prod[2][2] = trans1[2][0] * trans2[0][2] + trans1[2][1] * trans2[1][2] + trans1[2][2] * trans2[2][2];

    return;
}
