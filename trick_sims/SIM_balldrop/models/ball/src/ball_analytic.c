/*****************************************************************************
PURPOSE:    ( Analytical Ball )
*****************************************************************************/
#include <stdio.h>
#include <math.h>
#include "../include/ball_analytic.h"

int ball_analytic( BALL* B ) {

    if (B->gravity_change_command != 0) {
        B->gravity += B->gravity_change_command;
        B->gravity_change_command = 0;
    }

    B->acc = B->gravity ;
    B->vel = B->acc * B->time ;
    B->pos = B->height + ( 0.5 * B->acc * B->time * B->time ) ;
    
    if ( B->pos < 0.0 ) {
        
        B->impactTime = sqrt( ( 2 * B->height) / fabs(B->acc) ) ;
        B->pos = 0.0;
        B->vel = 0.0;
        
        if ( !B->impact ) {
            B->impact = 1;
            fprintf(stderr, "\n\nIMPACT: t = %.9f, acc = %.9f, vel = %.9f\n\n", B->impactTime, B->acc, B->vel ) ;
        }
    }
    /*
     * Increment time by the time delta associated with this job
     * Note that the 0.01 matches the frequency of this job
     * as specified in the S_define.
     */
    B->time += 0.01 ;
    return 0 ;
}