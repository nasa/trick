/********************************* TRICK HEADER *******************************
PURPOSE:
    (This routine runs at the begining of each targeting iteration loop on the
     Master side of the simulation.)
REFERENCE:
    (((None)))
ASSUMPTIONS AND LIMITATIONS:
    ((Many.))
CLASS:
    (monte_master_pre)
LIBRARY DEPENDENCY:
    ((target_master_pre.o))
PROGRAMMERS:
    (((Edwin Z. Crues) (NASA) (Nov 2009) (--) (Initial version.)))
*******************************************************************************/

/* System includes. */
#include <stdio.h>

/* Trick includes. */
#include "sim_services/include/exec_proto.h"
#include "trick_utils/math/include/vector_macros.h"

/* Model includes */
#include "../include/target_body.h"

/* ENTRY POINT */
int
target_master_pre(                 /* RETURN: -- Always returns zero.   */
   TargetBodyInit      * init,     /* INOUT: -- Iteration init data.    */
   TargetBodyState     * state,    /* INOUT: -- Iteration state data.   */
   TargetBodyData      * data,     /* INOUT: -- Targeting data.         */
   TargetBodyIteration * iterate ) /* INOUT: -- Iteration control data. */
{

   printf("\e[33mtarget_master_pre\(\)\e[00m\n");

   printf( "Initial Velocity:      x = %f km/s\n",
           state->velocity[0]/1000.0 );
   printf( "                       y = %f km/s\n",
           state->velocity[1]/1000.0 );
   printf( "                       z = %f km/s\n\n",
           state->velocity[2]/1000.0 );

   /* Return to calling function. */
   return( 0 );
}

