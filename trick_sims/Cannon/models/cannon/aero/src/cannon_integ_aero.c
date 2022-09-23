/****************************** TRICK HEADER ******************************
PURPOSE: (Cannon integration)
Tutorial Section 8
***************************************************************************/
#include <stdio.h>
#include "sim_services/Integrator/include/integrator_c_intf.h"
#include "../include/cannon_aero_proto.h"

int cannon_integ_aero(
  CANNON_AERO* C)
{

    int ipass;

    /* LOAD THE POSITION AND VELOCITY STATES */
    load_state(
        &C->pos[0] ,
        &C->pos[1] ,
        &C->pos[2] ,
        &C->vel[0] ,
        &C->vel[1] ,
        &C->vel[2] ,
        NULL
    );

    /* LOAD THE POSITION AND VELOCITY STATE DERIVATIVES */
    load_deriv(
        &C->vel[0] ,
        &C->vel[1] ,
        &C->vel[2] ,
        &C->acc[0] ,
        &C->acc[1] ,
        &C->acc[2] ,
        NULL
    );

    /* CALL THE TRICK INTEGRATION SERVICE */
    ipass = integrate();

    /* UNLOAD THE NEW POSITION AND VELOCITY STATES */
    unload_state(
        &C->pos[0] ,
        &C->pos[1] ,
        &C->pos[2] ,
        &C->vel[0] ,
        &C->vel[1] ,
        &C->vel[2] ,
        NULL
    );

    /* RETURN */
    return( ipass );
}
