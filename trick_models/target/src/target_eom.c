/********************************* TRICK HEADER *******************************
PURPOSE:
    (This routine computes the derivatives for the propagation of both the
     body state and the targeting state transition matrix.)
REFERENCE:
    (((None)))
ASSUMPTIONS AND LIMITATIONS:
    ((Dynamics are purely Keplerian.))
CLASS:
    (derivative)
LIBRARY DEPENDENCY:
    ((target_eom.o))
PROGRAMMERS:
    (((Edwin Z. Crues) (NASA) (Nov 2009) (--) (Initial version.)))
*******************************************************************************/

/* System includes. */

/* Trick includes. */
//#include "sim_services/include/integrator.h"
#include "trick_utils/math/include/trick_math.h"

/* Model includes */
#include "../include/target_body.h"

/* ENTRY POINT */
void
target_eom(                    /* RETURN: -- None.                   */
   TargetBodyPlanet * planet,  /* INOUT:  -- Target planet data.     */
   TargetBodyState  * state,   /* INOUT:  -- Target body state data. */
   TargetBodyData   * data   ) /* INOUT:  -- Targetting data.        */
{
   int iinc, jinc, kinc;
   double r_mag;
   double r_3, r_5;
   double outer[3][3];
   double mu = planet->mu;

   /* Compute the current radial distance. */
   r_mag = V_MAG( state->position );
   r_3 = r_mag * r_mag * r_mag;
   r_5 = r_3 * r_mag * r_mag;

   /*
    * Compute the time derivative of the state (acceleration).
    */
   V_SCALE( state->acceleration, state->position, (-mu / r_3) );

   /*
    * Compute the time derivative of the state transition matrix.
   */
   
   /* Compute the G matrix. */
   M_INIT( data->Gmat );
   for ( iinc = 0 ; iinc < 3 ; iinc++ ) {
      data->Gmat[iinc][iinc] = -mu / r_3;
   }
   V_OUTER( outer, state->position );
   MxSCALAR( outer, outer, ((3.0*mu)/r_5) );
   M_ADD( data->Gmat, data->Gmat, outer );

   /* Compute/compose the F matrix. */
   /* Upper left hand quadrant (zeros). */
   for ( iinc = 0 ; iinc < 3 ; iinc++ ) {
      for ( jinc = 0 ; jinc < 3 ; jinc++ ) {
         data->F[iinc][jinc] = 0.0;
      }
   }
   /* Upper right hand quadrant (identity). */
   for ( iinc = 0 ; iinc < 3 ; iinc++ ) {
      for ( jinc = 3 ; jinc < 6 ; jinc++ ) {
         data->F[iinc][jinc] = 0.0;
      }
      data->F[iinc][iinc+3] = 1.0;
   }
   /* Lower left hand quadrant (Gmat). */
   for ( iinc = 3 ; iinc < 6 ; iinc++ ) {
      for ( jinc = 0 ; jinc < 3 ; jinc++ ) {
         data->F[iinc][jinc] = data->Gmat[iinc-3][jinc];
      }
   }
   /* Lower right hand quadrant (zeros). */
   for ( iinc = 3 ; iinc < 6 ; iinc++ ) {
      for ( jinc = 3 ; jinc < 6 ; jinc++ ) {
         data->F[iinc][jinc] = 0.0;
      }
   }

   /* Compute the time derivative of the state transition matrix. */
   for ( iinc = 0 ; iinc < 6 ; iinc++ ) {
      for ( jinc = 0 ; jinc < 6 ; jinc++ ) {
         data->phidot[iinc][jinc] = 0.0;
         for ( kinc = 0 ; kinc < 6 ; kinc++ ) {
            data->phidot[iinc][jinc] += data->F[iinc][kinc] * data->phi[kinc][jinc];
         }
      }
   }

   return;

}

