/********************************* TRICK HEADER *******************************
PURPOSE:
      (Predict the path of the SUN across the sky.)
PROGRAMMERS:
      (((John M. Penn) (L-3 Titan) (Oct 24, 2010) (Trick Demo) ))
*******************************************************************************/
#include "sun_pred.h"

int sun_pred_default_data( SUN_PRED* S ) {

    S->observer_latitude  = 29.55298;
    S->observer_longitude = 95.09379;
    S->observer_offset_from_UTC = -6;
    S->local_time.year  = 2016;
    S->local_time.month = 3;
    S->local_time.day   = 11;
    S->local_time.hour  = 13;
    S->local_time.min   = 25;
    S->local_time.sec   = 0.0;
    /* Wide characters must use the prefix L when defined in quotes. */
    S->label_UTC        = L"UTC" ;
    S->label_JD         = L"JD" ;
    S->label_Azimuth    = L"A" ;
    S->label_Elevation  = L"h" ;

    return 0 ;
}

