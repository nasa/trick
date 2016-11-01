/*****************************************************************************
PURPOSE: ( Integration Job for SIM_cannon_numeric )
*****************************************************************************/
#include "../include/cannon.h"
#include "sim_services/Integrator/include/integrator_c_intf.h"

int cannon_integ( CANNON* C ) {

    int ipass;

    /* LOAD THE POSITION AND VELOCITY STATES */
    load_state(
        &C->pos[0] ,
        &C->pos[1] ,
        &C->vel[0] ,
        &C->vel[1] ,
        &C->time ,
        NULL
    );

    /* LOAD THE POSITION AND VELOCITY STATE DERIVATIVES */
    load_deriv(
        &C->vel[0] ,
        &C->vel[1] ,
        &C->acc[0] ,
        &C->acc[1] ,
        &C->timeRate ,
        NULL
    );

    /* CALL THE TRICK INTEGRATION SERVICE */
    ipass = integrate();

    /* UNLOAD THE NEW POSITION AND VELOCITY STATES */
    unload_state(
        &C->pos[0] ,
        &C->pos[1] ,
        &C->vel[0] ,
        &C->vel[1] ,
        &C->time ,
        NULL
    );

    /* RETURN */
    return( ipass );
}
