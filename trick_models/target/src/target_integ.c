/********************************* TRICK HEADER *******************************
PURPOSE:
    (This routine loads and calls the Trick integration routine to propagate
     both the target body state and the state transition matrix.)
REFERENCE:
    (((None)))
ASSUMPTIONS AND LIMITATIONS:
    ((Many.))
CLASS:
    (integration)
LIBRARY DEPENDENCY:
    ((target_integ.o))
PROGRAMMERS:
    (((Edwin Z. Crues) (NASA) (Nov 2009) (--) (Initial version.)))
*******************************************************************************/

/* System includes. */

/* Trick includes. */
#include "sim_services/Integrator/include/integrator_c_intf.h"
#include "trick_utils/math/include/trick_math.h"

/* Model includes */
#include "../include/target_body.h"

/* ENTRY POINT */
int target_integ(                /* RETURN: -- Integration mulit-pass pass id. */
   TargetBodyState * body,   /* INOUT:  -- Target body state data.         */
   TargetBodyData  * data  ) /* INOUT:  -- Targetting data.                */
{
   int      ipass;     

   /* Load the states and derivatives. */
   load_state(
         &body->position[0],
         &body->position[1],
         &body->position[2],
         &body->velocity[0],
         &body->velocity[1],
         &body->velocity[2],
         &data->phi[0][0],
         &data->phi[0][1],
         &data->phi[0][2],
         &data->phi[0][3],
         &data->phi[0][4],
         &data->phi[0][5],
         &data->phi[1][0],
         &data->phi[1][1],
         &data->phi[1][2],
         &data->phi[1][3],
         &data->phi[1][4],
         &data->phi[1][5],
         &data->phi[2][0],
         &data->phi[2][1],
         &data->phi[2][2],
         &data->phi[2][3],
         &data->phi[2][4],
         &data->phi[2][5],
         &data->phi[3][0],
         &data->phi[3][1],
         &data->phi[3][2],
         &data->phi[3][3],
         &data->phi[3][4],
         &data->phi[3][5],
         &data->phi[4][0],
         &data->phi[4][1],
         &data->phi[4][2],
         &data->phi[4][3],
         &data->phi[4][4],
         &data->phi[4][5],
         &data->phi[5][0],
         &data->phi[5][1],
         &data->phi[5][2],
         &data->phi[5][3],
         &data->phi[5][4],
         &data->phi[5][5],
         NULL
   );

   load_deriv(
         &body->velocity[0],
         &body->velocity[1],
         &body->velocity[2],
         &body->acceleration[0],
         &body->acceleration[1],
         &body->acceleration[2],
         &data->phidot[0][0],
         &data->phidot[0][1],
         &data->phidot[0][2],
         &data->phidot[0][3],
         &data->phidot[0][4],
         &data->phidot[0][5],
         &data->phidot[1][0],
         &data->phidot[1][1],
         &data->phidot[1][2],
         &data->phidot[1][3],
         &data->phidot[1][4],
         &data->phidot[1][5],
         &data->phidot[2][0],
         &data->phidot[2][1],
         &data->phidot[2][2],
         &data->phidot[2][3],
         &data->phidot[2][4],
         &data->phidot[2][5],
         &data->phidot[3][0],
         &data->phidot[3][1],
         &data->phidot[3][2],
         &data->phidot[3][3],
         &data->phidot[3][4],
         &data->phidot[3][5],
         &data->phidot[4][0],
         &data->phidot[4][1],
         &data->phidot[4][2],
         &data->phidot[4][3],
         &data->phidot[4][4],
         &data->phidot[4][5],
         &data->phidot[5][0],
         &data->phidot[5][1],
         &data->phidot[5][2],
         &data->phidot[5][3],
         &data->phidot[5][4],
         &data->phidot[5][5],
         NULL
   );

   /* Integrate state */
   ipass = integrate();

   /* Unload integrated state array. */
   unload_state(
         &body->position[0],
         &body->position[1],
         &body->position[2],
         &body->velocity[0],
         &body->velocity[1],
         &body->velocity[2],
         &data->phi[0][0],
         &data->phi[0][1],
         &data->phi[0][2],
         &data->phi[0][3],
         &data->phi[0][4],
         &data->phi[0][5],
         &data->phi[1][0],
         &data->phi[1][1],
         &data->phi[1][2],
         &data->phi[1][3],
         &data->phi[1][4],
         &data->phi[1][5],
         &data->phi[2][0],
         &data->phi[2][1],
         &data->phi[2][2],
         &data->phi[2][3],
         &data->phi[2][4],
         &data->phi[2][5],
         &data->phi[3][0],
         &data->phi[3][1],
         &data->phi[3][2],
         &data->phi[3][3],
         &data->phi[3][4],
         &data->phi[3][5],
         &data->phi[4][0],
         &data->phi[4][1],
         &data->phi[4][2],
         &data->phi[4][3],
         &data->phi[4][4],
         &data->phi[4][5],
         &data->phi[5][0],
         &data->phi[5][1],
         &data->phi[5][2],
         &data->phi[5][3],
         &data->phi[5][4],
         &data->phi[5][5],
         NULL
   );

   /* Return the integration step. */
   return ipass;
}

