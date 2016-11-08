#ifndef SUN_POS_H
#define SUN_POS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Calculate the Sun's position at the given Julian day number.
 * @param JD The Julian day number.
 * @param sun_right_ascension.
 * @param sun_declination.
 */
int sun_pos_at_JD (double JD, double *sun_right_ascension, double *sun_declination );

#ifdef __cplusplus
}
#endif

#endif
