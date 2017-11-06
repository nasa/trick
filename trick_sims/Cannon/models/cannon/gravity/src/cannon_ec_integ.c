/*****************************************************************************
PURPOSE: ( Integration Job for Euler-Cromer Example )
*****************************************************************************/

#include "trick/integrator_c_intf.h"
#include "../include/cannon.h"

int cannon_ec_integ( CANNON* C ) {

    int ipass;

    /* LOAD THE POSITION AND VELOCITY STATES */
    load_state(
        &C->pos[0] ,
        &C->pos[1] ,
        NULL
    );

    /* LOAD THE VELOCITY STATE DERIVATIVES */
    load_deriv(
        &C->vel[0] ,
        &C->vel[1] ,
        NULL
    );

    /* LOAD THE 2nd STATE DERIVATIVES */
    load_deriv2(
        &C->acc[0] ,
        &C->acc[1] ,
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
        NULL
    );

    /* RETURN */
    return( ipass );
}
