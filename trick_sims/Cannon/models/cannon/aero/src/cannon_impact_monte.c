/*********************************** TRICK HEADER **************************
PURPOSE:                     (Kaboom!!!)
***************************************************************************/
#include <stdio.h>
#include "../include/cannon_aero.h"
#include "../include/cannon_monte_proto.h"
#include "sim_services/Integrator/include/integrator_c_intf.h"
#include "sim_services/Executive/include/exec_proto.h"

double cannon_impact_monte( CANNON_AERO* C)
{
    double tgo ;
    double now ;

    /* Calculate time to go before impact */
    C->rf.error = C->pos[2] ;

    tgo = regula_falsi( get_integ_time() , &(C->rf) ) ;
    if (tgo == 0.0) {

        now = get_integ_time() ;
        /*  Ball impact */
        reset_regula_falsi( now , &(C->rf) ) ;

        C->vel[0] = 0.0 ; C->vel[1] = 0.0 ; C->vel[2] = 0.0 ;
        C->acc[0] = 0.0 ; C->acc[1] = 0.0 ; C->acc[2] = 0.0 ;
        C->g = 0.0 ;

        fprintf(stderr, "Impact time: %.9lf pos: %.9lf\n", now, C->pos[0]) ;
    }

    return( tgo ) ;
}
