/********************************* TRICK HEADER *******************************
PURPOSE:
    (This routine computes the change in delta-V required to adjust the
     initial velocity to hit the targeted position.)
REFERENCE:
    (((None)))
ASSUMPTIONS AND LIMITATIONS:
    ((Many.))
CLASS:
    (shutdown)
LIBRARY DEPENDENCY:
    ((target_delta_v.o))
PROGRAMMERS:
    (((Edwin Z. Crues) (NASA) (Nov 2009) (--) (Initial version.)))
*******************************************************************************/

/* System includes. */
#include <stdio.h>

/* Trick includes. */
#include "trick_utils/math/include/trick_math.h"
#include "sim_services/include/exec_proto.h"

/* Model includes */
#include "../include/target_body.h"

/* ENTRY POINT */
int
target_delta_v(                    /* RETURN: -- 0 - OK, 1 - Error.      */
   TargetBodyState     * state,    /* INOUT:  -- Target body state data. */
   TargetBodyData      * data,     /* INOUT:  -- Targetting data.        */
   TargetBodyIteration * iterate ) /* INOUT:  -- Iteration control data. */
{
   int iinc, jinc;

   double phi_12[3][3];
   double phi_12_inv[3][3];

   /* Compute the final distance from the desired target position. */
   V_SUB( iterate->delta_r, data->position, state->position );
   iterate->delta_r_mag = V_MAG( iterate->delta_r );

   /*
    * Calculate required change in initial velocity.
    */
   
   /* Extract the portion of the STM that we need. */
   for ( iinc = 0 ; iinc < 3 ; iinc++ ) {
      for ( jinc = 0 ; jinc < 3 ; jinc++ ) {
         phi_12[iinc][jinc] = data->phi[iinc][jinc+3];
      }
   }

   /* Invert the portion of the STM. */
   if ( dm_invert( phi_12_inv, phi_12 ) != TM_SUCCESS ) {
      printf( "File: \"%s\": STM has zero determinate!\n\n", __FILE__ );
      exec_terminate( __FILE__, "STM has zero determinate!" );
      return( 1 );
   }

   /* Compute the required change in velocity. */
   MxV( iterate->delta_v, phi_12_inv, iterate->delta_r );

   /* Return to calling function. */
   return( 0 );

}

