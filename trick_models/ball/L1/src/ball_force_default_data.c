/********************************* TRICK HEADER *******************************
PURPOSE:
    (Ball force default data initialization)
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

/*
 * $Id: ball_force_default_data.c 931 2010-08-10 20:32:30Z lin $
 */

#include <stdio.h>
#include "ball/L1/include/ball_force.h"
#include "ball/L1/include/ball_proto.h"

int ball_force_default_data(BFORCE * ball_force) {

    ball_force->input.origin[0] = 0.0 ;
    ball_force->input.origin[1] = 2.0 ;
    ball_force->input.force = 8.0 ;

    return(0) ;
}
