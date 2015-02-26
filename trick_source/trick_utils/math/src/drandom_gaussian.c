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
   PURPOSE: (Generate a random number consistent with a Gaussian distribution.)

   ASSUMPTIONS AND LIMITATIONS: ((State spaces for independent processes requiring random numbers are not tracked.)
   (Range is specified in standard deviations; i.e. if a range of 3 sigma is specified, then NO numbers greater than
   the plus/minus 3 sigma value will be returned.) (Automatic Seed)) PROGRAMMERS: (((Robert W. Bailey) (LinCom Corp)
   (4/15/91) (Trick-CR-00009) (SAFR Model))) */

/* 
 *  $Id: drandom_gaussian.c 49 2009-02-02 22:37:59Z lin $
 */

#include <stdlib.h>

#include "../include/trick_math.h"


#define MAX_VALUE 32768.0              /* 32768 = 2**15 */

double drandom_gaussian(        /* Return: Random number consistent with Gaussian distribution */
                           double std_deviation,        /* In: One standard deviation */
                           int range)
{                                      /* In: Plus/Minus range for Gaussian output in standard deviations */

    int i;
    double out;
    double number = 0.0;

    for (i = 0; i < 2 * range; i++) {
        /* Call rand() 2*range times to get plus/minus range sigma output */

        /* Normalize output of rand to the range of [-0.5,0.5] */
#ifdef __linux
        out = (((double) rand()) / RAND_MAX) - 0.5;
#elif __APPLE__
        out = (((double) rand()) / LONG_MAX) - 0.5;
#else
        out = (((double) rand()) / MAX_VALUE) - 0.5;
#endif

        /* Scale normalized output by one sigma value and sum to previous normalized values */
        number += (std_deviation * out);
    }

    /* Resulting summed value is a gaussian random number */
    return (number);
}
