/********************************* TRICK HEADER *******************************
PURPOSE:
    (This routine runs are the end of the targeting iteration sequence on the
     Slave side of the simulation.)
REFERENCE:
    (((None)))
ASSUMPTIONS AND LIMITATIONS:
    ((Many.))
CLASS:
    (monte_slave_shutdown)
LIBRARY DEPENDENCY:
    ((target_slave_shutdown.o))
PROGRAMMERS:
    (((Edwin Z. Crues) (NASA) (Nov 2009) (--) (Initial version.)))
*******************************************************************************/

/* System includes. */

/* Trick includes. */
#include "trick_utils/comm/include/tc_proto.h"

/* Model includes */
#include "../include/target_body.h"

/* ENTRY POINT */
int
target_slave_shutdown(       /* RETURN: -- Always returns zero.   */
   TCDevice       * tc_dev,  /* INOUT: -- Iteration comm device.  */
   TargetBodyData * data   ) /* INOUT: -- Targetting data.        */
{
   /* Return to calling function. */
   return( 0 );
}

