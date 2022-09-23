/*
PURPOSE:
    (Ball model right wall collision event job.)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((2-Dimensional space)
     (X-axis is horizontal and positive to the right)
     (Uses Trick regula falsi event service))
CLASS:
    (dynamic_event)
LIBRARY DEPENDENCY:
    ((ball_right_wall.o))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*/

#include <stdio.h>
#include "sim_services/Integrator/include/regula_falsi.h"
#include "sim_services/Integrator/include/integrator_c_intf.h"
#include "../include/ball_state.h"

/* ENTRY POINT: */
double ball_right_wall(   /* RETURN: s  Time to go to event */
    BSTATE * S )          /* INOUT:  -- Ball EOM state parameters */
{

    BSTATE_IN * SI = &(S->input) ;
    double tgo ;

    SI->right_wall.error = S->output.position[0] - SI->right_wall_x_pos ;
    tgo = regula_falsi( get_integ_time() , &(SI->right_wall) ) ;
    if ( tgo == 0.0 ) {
        double now = get_integ_time();
        reset_regula_falsi( now , &(SI->right_wall) ) ;
        S->output.velocity[0] = - S->output.velocity[0] ;
        printf("Hit Right wall @ t = %g.\n", now); fflush(stdout);
    }

return( tgo ) ;
}

