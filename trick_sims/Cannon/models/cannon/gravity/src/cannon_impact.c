/*****************************************************************************
PURPOSE: ( Dynamic Event to determine cannonball impact time. )
*****************************************************************************/
#include <stdio.h>
#include "../include/cannon.h"
#include "sim_services/Integrator/include/integrator_c_intf.h"

double cannon_impact( CANNON* C ) {

    double tgo ;
    double now ;

    /* Set error function --- how far ball is above ground */
    C->rf.error = C->pos[1] ;

    now = get_integ_time() ;
    tgo = regula_falsi( now, &(C->rf) ) ;

    if (tgo == 0.0) {
        /* Cannonball hits dirt and goes BOOM. */
        C->impact = 1 ;

        now = get_integ_time() ;
        reset_regula_falsi( now, &(C->rf) ) ;

        C->vel[0] = 0.0 ; C->vel[1] = 0.0 ;
        C->acc[0] = 0.0 ; C->acc[1] = 0.0 ;
        C->impactTime = C->time;

        fprintf(stderr, "\n\nIMPACT: SimTime = %.9f, ModelTime = %.9f, pos = %.9f\n\n", now, C->impactTime, C->pos[0] ) ;
    }

    return ( tgo ) ;
}
