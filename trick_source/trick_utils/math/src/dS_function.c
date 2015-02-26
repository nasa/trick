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
   PURPOSE: (Given an input value 'x', return a value 'y' in the range of zero to one, based on a function with the
   following properties: 1 - zero slope at zero and at one, 2 - continous, 3 - parabolic between at both ends, 4 - and
   not necessarily symmetric.)

   PROGRAMMERS: (((Robert W. Bailey) (LinCom Corp) (3/22/91) (Trick-CR-00000) (Initial release))) */

/* 
 * $Id: dS_function.c 49 2009-02-02 22:37:59Z lin $
 */

#include "../include/trick_math.h"

double dS_function(             /* Return: -- 0.0 <= value <= 1.0 */
                      double x, /* In: 'x' parameter */
                      double zero_point,        /* In: 'x' value where 's' function returns 0.0 */
                      double mid_point, /* In: 'x' value where 's' function returns 0.5 */
                      double one_point, /* In: 'x' value where 's' function returns 1.0 */
                      double sign)
{                                      /* In: 'x' axis sign evaluation flag: -1 = evaluate -'X' axis, 1 = evaluate +'X' 
                                          axis */

    double temp, temp1;
    double value;

    if (sign > 0) {
        if (x >= one_point)
            value = 1.0;
        else if (x <= zero_point)
            value = 0.0;
        else if (zero_point < x && x < mid_point) {
            temp = x - zero_point;
            temp1 = mid_point - zero_point;
            value = temp * temp / (2.0 * temp1 * temp1);
        } else {
            temp = x - one_point;
            temp1 = one_point - mid_point;
            value = 1.0 - (temp * temp / (2.0 * temp1 * temp1));
        }
    } else {
        if (x <= one_point)
            value = 1.0;
        else if (x >= zero_point)
            value = 0.0;
        else if (mid_point < x && x < zero_point) {
            temp = x - zero_point;
            temp1 = mid_point - zero_point;
            value = temp * temp / (2.0 * temp1 * temp1);
        } else {
            temp = x - one_point;
            temp1 = one_point - mid_point;
            value = 1.0 - (temp * temp / (2.0 * temp1 * temp1));
        }
    }

    return (value);
}
