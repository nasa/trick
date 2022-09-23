/********************************* TRICK HEADER *******************************
PURPOSE:
    (ball_state_integ performs the following:
        - loads the position states into the INTEGRATOR state ws arrays
        - loads the velocity states into the INTEGRATOR state derivative ws
        - loads the velocity states into the INTEGRATOR state ws arrays
        - loads the acceleration states into the INTEGRATOR state derivative ws
        - calls the TRICK state integration service
        - unloads the new pos and vel states from the INTEGRATOR ws arrays)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((2 dimensional space)
     (integrate accel to pos as two first order diffEQs))
CLASS:
    (integration)
LIBRARY DEPENDENCY:
    ((ball_state_integ.o))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

#include <stdio.h>

/* GLOBAL DATA STRUCTURE DECLARATIONS */
#include "sim_services/Integrator/include/integrator_c_intf.h"
#include "../include/ball_state.h"

/* ENTRY POINT */
int ball_state_integ( /* RETURN: -- Integration multi-step id */
  BSTATE *S )         /* INOUT:  -- Ball EOM state parameters */
{

    int ipass;

    /* GET SHORTHAND NOTATION FOR DATA STRUCTURES */
    BSTATE_OUT *SO = &(S->output) ;

    /* LOAD THE POSITION AND VELOCITY STATES */
    load_state(
        &SO->position[0] ,
        &SO->position[1] ,
        &SO->velocity[0] ,
        &SO->velocity[1] ,
        NULL
    );

    /* LOAD THE POSITION AND VELOCITY STATE DERIVATIVES */
    load_deriv(
        &SO->velocity[0] ,
        &SO->velocity[1] ,
        &SO->acceleration[0] ,
        &SO->acceleration[1] ,
        NULL
    );

    /* CALL THE TRICK INTEGRATION SERVICE */
    ipass = integrate();

    /* UNLOAD THE NEW POSITION AND VELOCITY STATES */
    unload_state(
        &SO->position[0] ,
        &SO->position[1] ,
        &SO->velocity[0] ,
        &SO->velocity[1] ,
        NULL
    );

    /* RETURN */
    return( ipass );
}

