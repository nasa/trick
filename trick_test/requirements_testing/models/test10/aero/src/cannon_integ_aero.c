/*********************************** TRICK HEADER **************************
PURPOSE:                     (Cannon integration)
***************************************************************************/

#include "sim_services/include/integrator.h" 
#include "../include/cannon_aero.h"

int cannon_integ_aero(
                  
  INTEGRATOR *I,
  CANNON_AERO* C) 
{ 

        /* LOAD STATES (Example Below) */
        I->state[0] = C->pos[0] ;
        I->state[1] = C->pos[1] ;
        I->state[2] = C->pos[2] ;
        I->state[3] = C->vel[0] ;
        I->state[4] = C->vel[1] ;
        I->state[5] = C->vel[2] ;

        /* LOAD DERIVATIVES (Example Below) */
        I->deriv[I->intermediate_step][0] = C->vel[0] ;
        I->deriv[I->intermediate_step][1] = C->vel[1] ;
        I->deriv[I->intermediate_step][2] = C->vel[2] ;
        I->deriv[I->intermediate_step][3] = C->acc[0] ;
        I->deriv[I->intermediate_step][4] = C->acc[1] ;
        I->deriv[I->intermediate_step][5] = C->acc[2] ;

        integrate( I ) ;

        /* UNLOAD NEW STATES (Example Below) */
        C->pos[0] = I->state_ws[I->intermediate_step][0] ;
        C->pos[1] = I->state_ws[I->intermediate_step][1] ;
        C->pos[2] = I->state_ws[I->intermediate_step][2] ;
        C->vel[0] = I->state_ws[I->intermediate_step][3] ;
        C->vel[1] = I->state_ws[I->intermediate_step][4] ;
        C->vel[2] = I->state_ws[I->intermediate_step][5] ;


        /* RETURN */
        return ( I->intermediate_step ) ;
}
