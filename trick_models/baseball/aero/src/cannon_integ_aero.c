/*********************************** TRICK HEADER **************************
PURPOSE:                     (Cannon integration)
***************************************************************************/

/*
 * $Id: cannon_integ_aero.c 737 2010-06-14 20:47:08Z wwoodard $
 */

#include "sim_services/Integrator/include/integrator_c_intf.h"
#include "../include/cannon_aero.h"

int cannon_integ_aero(
  CANNON_AERO* C)
{

    int ipass;

    /* GET SHORTHAND NOTATION FOR DATA STRUCTURES */
    CANNON_AERO_OUT *CAO = &(C->out) ;

    /* LOAD THE POSITION AND VELOCITY STATES */
    load_state(
        &CAO->position[0] ,
        &CAO->position[1] ,
        &CAO->position[2] ,
        &CAO->velocity[0] ,
        &CAO->velocity[1] ,
        &CAO->velocity[2] ,
        NULL
    );

    /* LOAD THE POSITION AND VELOCITY STATE DERIVATIVES */
    load_deriv(
        &CAO->velocity[0] ,
        &CAO->velocity[1] ,
        &CAO->velocity[2] ,
        &CAO->acceleration[0] ,
        &CAO->acceleration[1] ,
        &CAO->acceleration[2] ,
        NULL
    );

    /* CALL THE TRICK INTEGRATION SERVICE */
    ipass = integrate();

    /* UNLOAD THE NEW POSITION AND VELOCITY STATES */
    unload_state(
        &CAO->position[0] ,
        &CAO->position[1] ,
        &CAO->position[2] ,
        &CAO->velocity[0] ,
        &CAO->velocity[1] ,
        &CAO->velocity[2] ,
        NULL
    );

    /* RETURN */
    return( ipass );
}
