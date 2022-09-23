/********************************* TRICK HEADER *******************************
PURPOSE:
    ()
CLASS:
    (integration)
LIBRARY DEPENDENCY:
    ((ball_ensemble_integ.o))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

#include <stdio.h>

/* GLOBAL DATA STRUCTURE DECLARATIONS */
#include "sim_services/Integrator/include/integrator_c_intf.h"
#include "../include/ball_ensemble.h"

/* ENTRY POINT */
int ball_ensemble_integ( BSTATE *S[] ) {

    int ipass;

    /* GET SHORTHAND NOTATION FOR DATA STRUCTURES */
    BSTATE_OUT *SO0 = &(S[0]->output) ;
    BSTATE_OUT *SO1 = &(S[1]->output) ;

    /* LOAD THE POSITION AND VELOCITY STATES */
    load_state(
        &(SO0->position[0]) ,
        &(SO0->position[1]) ,
        &(SO0->velocity[0]) ,
        &(SO0->velocity[1]) ,
        &(SO1->position[0]) ,
        &(SO1->position[1]) ,
        &(SO1->velocity[0]) ,
        &(SO1->velocity[1]) ,
        NULL
    );

    /* LOAD THE POSITION AND VELOCITY STATE DERIVATIVES */
    load_deriv(
        &(SO0->velocity[0]) ,
        &(SO0->velocity[1]) ,
        &(SO0->acceleration[0]) ,
        &(SO0->acceleration[1]) ,
        &(SO1->velocity[0]) ,
        &(SO1->velocity[1]) ,
        &(SO1->acceleration[0]) ,
        &(SO1->acceleration[1]) ,
        NULL
    );

    /* CALL THE TRICK INTEGRATION SERVICE */
    ipass = integrate();

    /* UNLOAD THE NEW POSITION AND VELOCITY STATES */
    unload_state(
        &(SO0->position[0]) ,
        &(SO0->position[1]) ,
        &(SO0->velocity[0]) ,
        &(SO0->velocity[1]) ,
        &(SO1->position[0]) ,
        &(SO1->position[1]) ,
        &(SO1->velocity[0]) ,
        &(SO1->velocity[1]) ,
        NULL
    );

    /* RETURN */
    return( ipass );
}

