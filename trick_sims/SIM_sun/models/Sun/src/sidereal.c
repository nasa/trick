/*******************************************************************
 * Reference(s):
 * (1) Meeus,Jean."Astronomical Algorithms",
 * 2nd Edition, Willmann-Bell, Inc.
 * Chapter 12.
 *******************************************************************
 */
#include <stdio.h>
#include <math.h>
#include "JD.h"
#include "sidereal.h"

#define SECONDS_PER_DAY 86400

/* Calculate sidereal time at Greenwich. */
void GST_secs_time( double JD, double *GST ) {

     double T;
     double cap_theta_0;
     double theta_0;
     double JD_0;
     double UT;

     /* In the calculation for mean sidereal time (below)
      * T must correspond to 0h UT of the date. So, we
      * calculate JD_0 that corresponds to 0h.
      */
     if ( (JD-(int)JD) < 0.5) {
             JD_0 = (int)JD - 0.5;
     } else {
             JD_0 = (int)JD + 0.5;
     }

     UT = (JD - JD_0) * SECONDS_PER_DAY;

     /* Ref(1), equation (12.1). */
     /* Note that 2451545.0 is the J2000 epoch. */
     T = (JD_0 - 2451545.0) / 36525.0;

     /*  Ref(1), equation (12.2) */
     /*  Mean sidereal time (in seconds) at Greenwich at 0-h UT. */
     cap_theta_0 = 24110.54842 +
                   (8640184.812866 * T) +
                   (0.093104 * T * T) -
                   (0.0000062 * T * T * T);

     /* We want it in the range 0..86400. */
     /* Note that there are 84600 seconds in a day. */
     if (cap_theta_0 >= 0.0) {
          cap_theta_0 = fmod(cap_theta_0, 86400.0);
     } else {
          cap_theta_0 = fmod(cap_theta_0, 86400.0) + 86400.0;
     }

     /*  Ref(1), page 87 */
     theta_0 = UT * 1.00273790935 + cap_theta_0;

     if (theta_0 >= 0.0) {
          theta_0 = fmod(theta_0, 86400.0);
     } else {
          theta_0 = fmod(theta_0, 86400.0) + 86400.0;
     }

     *GST = theta_0;

}

void LST_secs_time( double GST, double longitude, double *LST) {

        *LST = GST - (longitude * (3600.0/15.0));
}












