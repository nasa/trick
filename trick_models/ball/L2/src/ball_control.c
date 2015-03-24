/********************************* TRICK HEADER *******************************
PURPOSE:
    (Ball model altitude control system job.)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((2 jets)
     (jet 0 fires down)
     (jet 1 fires up)
     (simple bang-bang phase plane))
CLASS:
    (scheduled)
LIBRARY DEPENDENCY:
    ((ball_control.o))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

#include <math.h> /* FOR M_ABS() function */

#include "sim_services/Executive/include/exec_proto.h" /* For exec_get_job_cycle() */
#include "trick_utils/math/include/trick_math.h" /* For M_ABS */

#include "../include/ball_control.h"

int ball_control(
                     /* RETURN: -- Always return zero */
BCONTROL *C ,        /* INOUT: -- Ball altitude control system parameters */
double    altitude ) /* IN:    M  Ball jet commands from the flight software */
{

    BCONTROL_IN * CI = &(C->input) ;
    BCONTROL_OUT * CO = &(C->output) ;
    BCONTROL_WORK * CW = &(C->work) ;

    double alt_error ;

        /* INITIALIZE JET COMMANDS */
    CO->jet_command[0] = Off ;
    CO->jet_command[1] = Off ;

    if( CI->active == Yes ) {
        /* IF THE CONTROL SYSTEM IS ACTIVE... */

        /* COMPUTE ALTITUDE ERROR AND ESTIMATED ALTITUDE RATE OF CHANGE */
        alt_error = CI->target_altitude - altitude ;
        CW->rate_est = ( altitude - CW->last_altitude ) / exec_get_job_cycle(NULL) ;
        CW->last_altitude = altitude ;

        if( M_ABS( CW->rate_est ) > CI->max_rate ) {
            /* ALTITUDE EXCEEDING MAX_RATE */
            if( CW->rate_est > 0.0 )
                CO->jet_command[0] = On ;
            else CO->jet_command[1] = On ;
        }
        else if( M_ABS( alt_error ) > CI->dead_zone ) {
            /* ALTITUDE ERROR OUTSIDE OF DEAD ZONE */
            if( alt_error > 0.0 )
                CO->jet_command[1] = On ;
            else CO->jet_command[0] = On ;
        }
    }

    return( 0 ) ;
}

