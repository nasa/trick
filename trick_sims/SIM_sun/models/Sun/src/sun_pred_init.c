/********************************* TRICK HEADER *******************************
PURPOSE:
      (Predict the path of the SUN across the sky.)
LIBRARY DEPENDENCY:
      ((JD.o))
PROGRAMMERS:
      (((John M. Penn) (L-3 Titan) (Oct 8, 2006) (Trick Demo) ))
*******************************************************************************/
#include "sun_pred.h"
#include "JD.h"

#define HOURS_PER_DAY    24.0
#define MINUTES_PER_DAY  1440.0
#define SECONDS_PER_DAY  86400.0

int sun_pred_init( SUN_PRED* S ) {

    double dday;
    double JDL;
    double tsec;

    /* Convert the local time to JD */
    dday = S->local_time.day +
        ( S->local_time.hour /  HOURS_PER_DAY   ) +
        ( S->local_time.min  /  MINUTES_PER_DAY ) +
        ( S->local_time.sec  /  SECONDS_PER_DAY ) ;

    Calendar_Date_to_JD (S->local_time.year, S->local_time.month, dday, &JDL);

    /* Subtract the local offset from UTC from the JD. */

    S->JD_start = JDL - ( S->observer_offset_from_UTC / HOURS_PER_DAY ) ;
    S->JD = S->JD_start;

    /* Convert the new JD back to UTC Calendar date */
    JD_to_Calendar_Date (S->JD, &S->utc.year, &S->utc.month, &dday );

    S->utc.day  = (int)dday;
    tsec = (dday - S->utc.day ) * SECONDS_PER_DAY + 0.5;
    S->utc.hour = (int)( tsec / 3600.0 ); tsec = tsec - ( S->utc.hour * 3600.0 );
    S->utc.min  = (int)( tsec / 60.0 );   tsec = tsec - ( S->utc.min  * 60.0 );
    S->utc.sec  = (int)( tsec );

    return 0 ;
}

