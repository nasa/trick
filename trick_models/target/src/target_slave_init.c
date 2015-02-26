/********************************* TRICK HEADER *******************************
PURPOSE:
    (This routine initializes the targeting iteration loop on the Slave side
     of the simulation.)
REFERENCE:
    (((None)))
ASSUMPTIONS AND LIMITATIONS:
    ((Many.))
CLASS:
    (monte_slave_init)
LIBRARY DEPENDENCY:
    ((target_slave_init.o))
PROGRAMMERS:
    (((Edwin Z. Crues) (NASA) (Nov 2009) (--) (Initial version.)))
*******************************************************************************/

/* System includes. */

/* Trick includes. */
#include "sim_services/include/exec_proto.h"

/* Model includes */
#include "../include/target_body.h"

/* ENTRY POINT */
int
target_slave_init(                 /* RETURN: -- Always returns zero.   */
   TargetBodyIteration * iterate ) /* INOUT: -- Iteration control data. */
{
   /* Return to calling function. */
   return( 0 );
}

