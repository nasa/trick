/********************************* TRICK HEADER *******************************
PURPOSE:
    (This routine runs at the begining of each targeting iteration loop on the
     Slave side of the simulation.)
REFERENCE:
    (((None)))
ASSUMPTIONS AND LIMITATIONS:
    ((Many.))
CLASS:
    (monte_slave_pre)
LIBRARY DEPENDENCY:
    ((target_slave_pre.o))
PROGRAMMERS:
    (((Edwin Z. Crues) (NASA) (Nov 2009) (--) (Initial version.)))
*******************************************************************************/

/* System includes. */
#include <stdio.h>

/* Trick includes. */
#include "sim_services/include/exec_proto.h"

/* Model includes */
#include "../include/target_body.h"

/* ENTRY POINT */
int
target_slave_pre(                  /* RETURN: -- Always returns zero.   */
   TargetBodyInit      * init,     /* INOUT: -- Target init state data. */
   TargetBodyState     * state,    /* INOUT: -- Target body state data. */
   TargetBodyData      * data,     /* INOUT: -- Targeting data.         */
   TargetBodyIteration * iterate ) /* INOUT: -- Iteration control data. */
{

   printf( "Initial Velocity:      x = %f km/s\n",
           state->velocity[0]/1000.0 );
   printf( "                       y = %f km/s\n",
           state->velocity[1]/1000.0 );
   printf( "                       z = %f km/s\n\n",
           state->velocity[2]/1000.0 );

   /* Return to calling function. */
   return( 0 );
}

