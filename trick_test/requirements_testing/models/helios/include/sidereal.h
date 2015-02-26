#ifndef SIDEREAL_H
#define SIDEREAL_H

#ifdef __cplusplus
extern "C" {
#endif

void GST_secs_time( double JD, double *GST );
void LST_secs_time( double GST, double longitude, double *LST );

#ifdef __cplusplus
}
#endif

#endif

