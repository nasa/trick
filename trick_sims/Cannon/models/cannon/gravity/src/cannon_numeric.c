/*****************************************************************************
PURPOSE: ( Numeric integration for SIM_cannon_numeric )
*****************************************************************************/
#include <stdio.h>
#include "../include/cannon_numeric.h"
#include "trick/integrator_c_intf.h"

/* The cannonball sim's derivative job */
int cannon_deriv( CANNON* C ) {

      if (!C->impact) {
          C->acc[0] =  0.0;
          C->acc[1] = -C->g;
          C->timeRate = 1.00;
      }
      return 0 ; 
}

/* The cannonball sim's integration job */
int cannon_integ( CANNON* C ) {

    int ipass;
    /* LOAD THE POSITION AND VELOCITY STATES */
    load_state(
        &C->pos[0], &C->pos[1] ,
        &C->vel[0], &C->vel[1] ,
        &C->time,
        NULL
    );
    /* LOAD THE POSITION AND VELOCITY STATE DERIVATIVES */
    load_deriv(
        &C->vel[0], &C->vel[1] ,
        &C->acc[0], &C->acc[1] ,
        &C->timeRate,
        NULL
    );
    /* CALL THE TRICK INTEGRATION SERVICE */
    ipass = integrate();
    /* UNLOAD THE NEW POSITION AND VELOCITY STATES */
    unload_state(
        &C->pos[0], &C->pos[1] ,
        &C->vel[0], &C->vel[1] ,
        &C->time,
        NULL
    );
    /* RETURN */
    return( ipass );
}

/* The cannonball sim's dynamic job */
double cannon_impact( CANNON* C ) {
    double tgo ; /* time-to-go */
    double now ; /* current integration time. */
    
    C->rf.error = C->pos[1] ;              /* Specify the event boundary. */
    now = get_integ_time() ;               /* Get the current integration time */
    tgo = regula_falsi( now, &(C->rf) ) ;  /* Estimate remaining integration time. */ 
    if (tgo == 0.0) {                      /* If we are at the event, it's action time! */
        now = get_integ_time() ;
        reset_regula_falsi( now, &(C->rf) ) ; 
        C->impact = 1 ;
        C->impactTime = now ;
        C->vel[0] = 0.0 ; C->vel[1] = 0.0 ;
        C->acc[0] = 0.0 ; C->acc[1] = 0.0 ;
        fprintf(stderr, "\n\nIMPACT: SimTime = %.9f, pos = %.9f\n\n", now, C->pos[0] ) ;
    }
    return (tgo) ;
}
