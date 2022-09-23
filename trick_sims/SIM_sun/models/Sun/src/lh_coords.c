/*******************************************************************
 * Reference(s):
 * (1) Meeus,Jean."Astronomical Algorithms",
 * 2nd Edition, Willmann-Bell, Inc.
 * Chapter 13.
 *******************************************************************
 */

#include <math.h>
#include "lh_coords.h"

#ifndef PI
#define PI 3.14159264
#endif
#define DEG_TO_RAD (PI/180.0)
#define RAD_TO_DEG (180.0/PI)

void equatorial_to_local_horiz(double lat,   /* Observer's latitude       IN */
                               double HA,    /* hour_angle                IN */
                               double ra,    /* Right Ascension (degrees) IN */
                               double decl,  /* Declination (degrees)     IN */
                               double *az,   /* Azimuth (degrees)         OUT */
                               double *el) { /* Elevation (degrees)       OUT */

     double HA_r   = HA   * DEG_TO_RAD;
     double lat_r  = lat  * DEG_TO_RAD;
     double decl_r = decl * DEG_TO_RAD;

     /* Ref(1), (13.5) */
     *az = RAD_TO_DEG * atan2 ( sin(HA_r) , ( cos(HA_r) * sin(lat_r) - tan (decl_r) * cos(lat_r) ) );
     /* Ref(1), (13.6) */
     *el = RAD_TO_DEG * asin ( sin(lat_r) * sin(decl_r) + cos(lat_r)*cos(decl_r)*cos(HA_r) );

}

