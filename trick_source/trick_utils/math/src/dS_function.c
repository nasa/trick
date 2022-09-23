/*
   PURPOSE: (Given an input value 'x', return a value 'y' in the range of zero to one, based on a function with the
   following properties: 1 - zero slope at zero and at one, 2 - continous, 3 - parabolic between at both ends, 4 - and
   not necessarily symmetric.)

   PROGRAMMERS: (((Robert W. Bailey) (LinCom Corp) (3/22/91) (Trick-CR-00000) (Initial release))) */

#include "trick/trick_math.h"

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
