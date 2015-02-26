/********************************* TRICK HEADER *******************************
PURPOSE:
    (Ball state default data.)
PROGRAMMERS:
   (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

/*
 * $Id: ball_state_default_data.c 931 2010-08-10 20:32:30Z lin $
 */

#include "trick_utils/units/include/constant.h"
#include "ball/L1/include/ball_state.h"
#include "ball/L1/include/ball_proto.h"

int ball_state_default_data( BSTATE * ball_state ) {

    ball_state->input.mass = 10.0 ;
    ball_state->input.speed = 3.5 ;
    ball_state->input.elevation = 45.0 * DTR ;
    ball_state->input.position[0] = 5.0 ;
    ball_state->input.position[1] = 5.0 ;

    return(0) ;
}

