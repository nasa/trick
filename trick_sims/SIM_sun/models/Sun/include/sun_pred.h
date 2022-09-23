/*************************************************************************
PURPOSE: (SUN sim)
**************************************************************************/

#ifndef SUN_PRED_H
#define SUN_PRED_H
#include <wchar.h>

typedef struct {
    int year;                         /* (-- )  Year (-4712 .. INT_MAX) */
    int month;                        /* (-- )  Month (Jan=1, ... Dec=12) */
    int day;                          /* (day)  Day of the month (1..31) */
    int hour;                         /* (hr )  Hour of the day  (0..23) */
    int min;                          /* (min)  Minute of the hour (0..59) */
    double sec;                       /* (s  )  Second of the minute (0..59) */
} CALENDAR_DATE;

typedef struct {
   double         observer_latitude;          /* (degree) */
   double         observer_longitude;         /* (degree)  */
   CALENDAR_DATE  local_time;                 /* (--) */
   double         local_sidereal_time;        /* (s)  */
   double         sidereal_time_at_Greenwich; /* (s)  */
   int            observer_offset_from_UTC;   /* (hr) */
   CALENDAR_DATE  utc;                        /* (--) */
   double         JD_start;        /* (day)  Julian date at the beginning of the sim run.*/
   double         JD;              /* (day)  Current Julian date */
   double         right_ascension; /* (degree)  */
   double         declination;     /* (degree)  */
   double         hour_angle;      /* (degree)  */
   double         solar_azimuth;   /* (degree)  */
   double         solar_elevation; /* (degree)  */
   wchar_t*       label_UTC;       /* (--) wide character pointer */
   wchar_t*       label_JD;        /* (--) wide character pointer */
   wchar_t*       label_Azimuth;   /* (--) wide character pointer */
   wchar_t*       label_Elevation; /* (--) wide character pointer */
} SUN_PRED;

#ifdef __cplusplus
extern "C" {
#endif
int sun_pred_default_data( SUN_PRED* S ) ;
int sun_pred_cyclic( SUN_PRED* S, double current_sim_time ) ;
int sun_pred_init( SUN_PRED* S ) ;
int sun_pred_shutdown( void ) ;
int sun_pred_slow_display( SUN_PRED* S ) ;
int sun_pred_fast_display( SUN_PRED* S ) ;
#ifdef __cplusplus
}
#endif


#endif
