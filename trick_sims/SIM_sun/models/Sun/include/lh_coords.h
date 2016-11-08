#ifndef LH_COORDS_H
#define LH_COORDS_H

#ifdef __cplusplus
extern "C" {
#endif

void equatorial_to_local_horiz(double lat,   /* Observer's latitude       IN */
                               double HA,    /* hour_angle                IN */
                               double ra,    /* Right Ascension (degrees) IN */
                               double decl,  /* Declination (degrees)     IN */
                               double *az,   /* Azimuth (degrees)         OUT */
                               double *el) ; /* Elevation (degrees)       OUT */

#ifdef __cplusplus
}
#endif

#endif



