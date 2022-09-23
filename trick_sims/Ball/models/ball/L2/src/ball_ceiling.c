/*
PURPOSE:
    (Ball model ceiling collision event job.)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((2-Dimensional space)
     (Y-axis is vertical and positive up)
     (Uses Trick regula falsi event service))
CLASS:
    (dynamic_event)
LIBRARY DEPENDENCY:
    ((ball_ceiling.o))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*/

#include <stdio.h>
#include "sim_services/Integrator/include/regula_falsi.h"
#include "sim_services/Integrator/include/integrator_c_intf.h"
#include "../include/ball_state.h"

/* ENTRY POINT: */
double ball_ceiling( /* RETURN: s  Time to go to event */
    BSTATE * S )     /* INOUT:  -- Ball EOM state parameters */
{

    BSTATE_IN * SI = &(S->input) ;
    double tgo ;

    SI->ceiling.error = S->output.position[1] - SI->ceiling_y_pos ;
    tgo = regula_falsi( get_integ_time() , &(SI->ceiling) ) ;
    if (tgo == 0.0 ) {
        double now = get_integ_time();
        reset_regula_falsi( now , &(SI->ceiling) ) ;
        S->output.velocity[1] = - S->output.velocity[1] ;
        printf("Hit Ceiling @ t = %g.\n", now); fflush(stdout);
    }
    return( tgo ) ;
}

