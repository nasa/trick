/********************************* TRICK HEADER *******************************
PURPOSE:
      (Predict the path of the SUN across the sky.)
LIBRARY DEPENDENCY:
      ((sun_pos.o)(lh_coords.o)(sidereal.o)
PROGRAMMERS:
      (((John M. Penn) (L-3 Titan) (Oct 8, 2006) (Trick Demo) ))
*******************************************************************************/
#include <stdio.h>
#include <math.h>
#include "JD.h"
#include "sun_pred.h"
#include "sun_pos.h"
#include "lh_coords.h"
#include "sidereal.h"

#define HOURS_PER_DAY    24.0
#define MINUTES_PER_DAY  1440.0
#define SECONDS_PER_DAY  86400.0

int sun_pred_cyclic( SUN_PRED* S,
                     double    current_sim_time ) {

       double dday;
       double JDL;
       double tsec;

       /* Update the Julian date by one second. */
       S->JD = S->JD_start + ( current_sim_time / SECONDS_PER_DAY );


       /* Convert the new JD to UTC Calendar date. */
       JD_to_Calendar_Date (S->JD, &S->utc.year, &S->utc.month, &dday );

       S->utc.day  = (int)dday;
       tsec = (dday - S->utc.day ) * SECONDS_PER_DAY + 0.5;
       S->utc.hour = (int)( tsec / 3600.0 ); tsec = tsec - ( S->utc.hour * 3600.0 );
       S->utc.min  = (int)( tsec / 60.0 );   tsec = tsec - ( S->utc.min  * 60.0 );
       S->utc.sec  = (int)( tsec );


       /* Convert the new JD to Local Calendar date. */
       JDL = S->JD + ( S->observer_offset_from_UTC / HOURS_PER_DAY ) ;

       JD_to_Calendar_Date (JDL, &S->local_time.year, &S->local_time.month, &dday );

       S->local_time.day  = (int)dday;
       tsec = (dday - S->local_time.day ) * SECONDS_PER_DAY + 0.5;
       S->local_time.hour = (int)( tsec / 3600.0 ); tsec = tsec - ( S->local_time.hour * 3600.0 );
       S->local_time.min  = (int)( tsec / 60.0 );   tsec = tsec - ( S->local_time.min  * 60.0 );
       S->local_time.sec  = (int)( tsec );

       /* Calculate the sidereal time at Greenwich. */
       GST_secs_time( S->JD, &S->sidereal_time_at_Greenwich );


       /* Calculate the local sidereal time. */
       LST_secs_time( S->sidereal_time_at_Greenwich,
                      S->observer_longitude, &S->local_sidereal_time );


       /* Calculate the right_ascension and declination of the Sun. */
       if ( sun_pos_at_JD (S->JD, &S->right_ascension, &S->declination) < 0 ){
               return -1;
       }

       /* Calculate the local hour angle of the Sun. */
       S->hour_angle = S->sidereal_time_at_Greenwich * ( 360.0 / 86400.0 ) -
               S->observer_longitude - S->right_ascension;


       /* Calculate the local azimuth and elevation of the Sun. */
       equatorial_to_local_horiz(S->observer_latitude,
                                 S->hour_angle,
                                 S->right_ascension,
                                 S->declination,
                                 &S->solar_azimuth,
                                 &S->solar_elevation);


       /* The previous function reckons azimuth from south.
        * We want to reckon it from north.
        */
       S->solar_azimuth += 180.0;

       return 0;
}

