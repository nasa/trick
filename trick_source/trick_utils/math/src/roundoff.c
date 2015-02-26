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
   PURPOSE: (Round off a double precision value to the resolution specified.) REFERENCE: ((None)) ASSUMPTIONS AND
   LIMITATIONS: ((none)) CLASS: (N/A) LIBRARY DEPENDENCY: ((roundoff.o)) PROGRAMMERS: (((Robert W. Bailey) (LinCom
   Corp) (Feb 1991) (v1.0) (Initial Release.))) */

/* 
 * $Id: roundoff.c 49 2009-02-02 22:37:59Z lin $
 */

#include "../include/trick_math.h"

double roundoff(                /* Return: Value after roundoff */
                   double res,  /* In: Resolution for roundoff function */
                   double val)
{                                      /* In: Value to be truncated */

    double remain;

    remain = fmod(val, res);

    if ((remain / res) <= 0.5)
        return (val - remain);
    else
        return (val + ((1.0 * res) - remain));

}
