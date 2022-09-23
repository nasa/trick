/*******************************************************************
 * Reference(s):
 * (1) Meeus,Jean."Astronomical Algorithms",
 * 2nd Edition, Willmann-Bell, Inc.
 * Chapter 25.
 *******************************************************************
 */
#include <math.h>
#include "sun_pos.h"

#ifndef PI
#define PI 3.14159264
#endif
#define DEG_TO_RAD (PI/180.0)
#define RAD_TO_DEG (180.0/PI)

static double norm_mod(double x, double y) {
     if (x >= 0.0) {
             x = fmod(x, y);
     } else {
             x = fmod(x, y) + y;
     }
     return x;
}

/**
 * Calculate the position of the Sun (right ascension and declination )
 * @param JD Julian Date
 * @param sun_right_ascension Right ascension of the Sun
 * @param sun_declination Declination of the Sun
 */
int sun_pos_at_JD (double JD,
                   double *sun_right_ascension,
                   double *sun_declination ) {

     /* We are ignoring nutation in this calculation. */

     double T;
     double L_0;
     double M;
     double M_rad;
     double C;
     double Suns_true_longitude;
     double epsilon;
     double tmp_d;
     double right_ascension;
     double declination;

     /* Ref(1), (25.1) */
     T = (JD - 2451545.0) / 36525.0;

     /* Geometric mean longitude of the Sun. Ref(1), (25.2) */
     L_0 = 280.46646 + T * (36000.76983 +  T * 0.0003032 ) ;
     L_0 = norm_mod(L_0, 360.0);

     /* Mean anomaly of the Sun. Ref(1), (25.3) */
     M = 357.52911 + T * (35999.05029 +  T * 0.0001537 ) ;
     M = norm_mod(M, 360.0);

     M_rad = M * DEG_TO_RAD;

     C = ( 1.914602 - T * ( 0.004817 - T * 0.000014 ) ) * sin( M_rad )
       + ( 0.019993 - T * 0.000101 ) * sin (2 * M_rad)
       + 0.000289  * sin (3 * M_rad) ;

     Suns_true_longitude = L_0 + C;

     epsilon = 23.4392911; /* Ref(1), page 92 */

     tmp_d = cos( epsilon * DEG_TO_RAD ) * sin(Suns_true_longitude * DEG_TO_RAD) ;
     right_ascension = RAD_TO_DEG * atan2( tmp_d, cos(Suns_true_longitude * DEG_TO_RAD) );
     right_ascension = norm_mod(right_ascension, 360.0);

     tmp_d = sin( epsilon * DEG_TO_RAD ) * sin (Suns_true_longitude * DEG_TO_RAD);
     declination = RAD_TO_DEG * asin (tmp_d);

     *sun_right_ascension = right_ascension;
     *sun_declination     = declination;

     return 0;
}
