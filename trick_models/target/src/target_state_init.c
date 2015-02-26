/********************************* TRICK HEADER *******************************
PURPOSE:
    (This routine initializes the targeting data structure.)
REFERENCE:
    (((None)))
ASSUMPTIONS AND LIMITATIONS:
    ((Many.))
CLASS:
    (initialization)
LIBRARY DEPENDENCY:
    ((target_state_init.o))
PROGRAMMERS:
    (((Edwin Z. Crues) (NASA) (Nov 2009) (--) (Initial version.)))
*******************************************************************************/

/* System includes. */
#include <math.h>
#include <stdio.h>

/* Trick includes. */
#include "trick_utils/math/include/trick_math.h"

/* Model includes */
#include "../include/target_body.h"

/* ENTRY POINT */
void
target_state_init(             /* RETURN: -- None.                       */
   TargetBodyInit   * init,    /* IN:     -- Target initialization data. */
   TargetBodyPlanet * planet,  /* INOUT:  -- Target planet data.         */
   TargetBodyState  * state  ) /* INOUT:  -- Target body state data.     */
{
   double r_mag;
   double mu = planet->mu;

   /* Copy the initial states into the target state init vectors. */
   V_COPY( init->position, state->position );
   V_COPY( init->velocity, state->velocity );

   /* Just fill in the acceleration as a starting point. */
   r_mag = V_MAG( state->position );
   V_INIT( state->acceleration );
   V_SCALE( state->acceleration, state->position, (-mu/(r_mag*r_mag*r_mag)) );

   /* Let's print out some debug information. */
   printf( "-----------------------------------------\n" );
   printf( "Initial Position:      x = %f km\n",
           init->position[0]/1000.0 );
   printf( "                       y = %f km\n",
           init->position[1]/1000.0 );
   printf( "                       z = %f km\n\n",
           init->position[2]/1000.0 );
   printf( "Initial Velocity:      x = %f km/s\n",
           init->velocity[0]/1000.0 );
   printf( "                       y = %f km/s\n",
           init->velocity[1]/1000.0 );
   printf( "                       z = %f km/s\n\n",
           init->velocity[2]/1000.0 );

   return;
}

