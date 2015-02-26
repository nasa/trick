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
   PURPOSE: (To orthanormalize a 3X3 transformation matrix.)

   ASSUMPTIONS AND LIMITATIONS: ((One axis of transformation matrix is forfieted.) (This axis rotates between first,
   second, and third axes.))

   PROGRAMMERS: (((Robert W. Bailey) (LinCom Corp) (4/1/91) (Trick-CR-00004) (Gravity / Inertial Frame Models))) */

/* 
 * $Id: dm_orthonormal.c 49 2009-02-02 22:37:59Z lin $
 */

#include "../include/trick_math.h"

void dm_orthonormal(            /* Return: -- none */
                       double m[3][3],  /* Inout: Matrix to be orthonormalized. */
                       int *axis)
{                                      /* Inout: Axis to forfiet during orthonormalization */

    double vec[3];              /* Scratch space for taking unit vectors */

    switch (*axis) {
        case 0:
            /* Unitize the second and third rows */
            /* Cross the second and third rows to get the first */
            V_NORM(vec, m[1]);
            V_COPY(m[1], vec);
            V_NORM(vec, m[2]);
            V_COPY(m[2], vec);
            V_CROSS(m[0], m[1], m[2]);
            (*axis)++;
            break;

        case 1:
            /* Unitize the third and first rows */
            /* Cross the third and first rows to get the second */
            V_NORM(vec, m[2]);
            V_COPY(m[2], vec);
            V_NORM(vec, m[0]);
            V_COPY(m[0], vec);
            V_CROSS(m[1], m[2], m[0]);
            (*axis)++;
            break;

        case 2:
            /* Unitize the first and second rows */
            /* Cross the first and second rows to get the third */
            V_NORM(vec, m[0]);
            V_COPY(m[0], vec);
            V_NORM(vec, m[1]);
            V_COPY(m[1], vec);
            V_CROSS(m[2], m[0], m[1]);
            *axis = 0;
            break;
    }

    return;
}
