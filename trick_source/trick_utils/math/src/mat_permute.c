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
   PURPOSE: (Arrange a system of equations of the form Ax=B according to a different DOF sequence)

   REFERENCE: ((Les Quiocho) (NASA/JSC))

   ASSUMPTIONS AND LIMITATIONS: ((New DOF sequence is fixed) (A matrix is full))

   PROGRAMMERS: (((Les Quiocho) (NASA/JSC) (September 93) (Trick-CR-xxxxx) (--))) */

/* 
 * $Id: mat_permute.c 49 2009-02-02 22:37:59Z lin $
 */

#include "../include/trick_math.h"

void mat_permute(               /* Return: -- none */
                    double **A_coeff,   /* In: A_coeff */
                    double *B_coeff,    /* In: B_coeff */
                    double **A_star,    /* Out: A_star */
                    double *B_star,     /* Out: B_star */
                    int *new_dof,       /* In: new dof arrangement */
                    int num_states)
{                                      /* In: num states */

    int i, j, k;

    /* Reformat rhs vector according to specified dof sequence */
    for (i = 0; i < num_states; i++)
        for (k = 0; k < num_states; k++)
            if (k == new_dof[i])
                B_star[i] = B_coeff[k];

    /* Reformat lhs mass matrix to correspond with new dof sequence */
    for (i = 0; i < num_states; i++)
        for (j = 0; j < num_states; j++)
            A_star[i][j] = A_coeff[new_dof[i]][new_dof[j]];

    return;

}
