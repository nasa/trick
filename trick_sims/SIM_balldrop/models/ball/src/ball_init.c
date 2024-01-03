/******************************* TRICK HEADER ****************************
PURPOSE: (Set the initial data values)
*************************************************************************/

/* Model Include files */
#include <math.h>
#include "../include/ball.h"

/* default data job */
int ball_default_data( BALL* B ) {
    
    B->gravity = -0.0127;
    B->height = 22.86;

    B->time = 0.0 ;

    B->impact = 0 ;
    B->impactTime = 0.0 ;

    return 0 ;
}

/* initialization job */
int ball_init( BALL* B ) {
   
    B->vel = 0 ;
    B->pos = B->height ;
    B->acc = B->gravity ;
    
    B->impactTime = 0.0 ;
    B->impact = 0.0 ;

    return 0 ;
}
