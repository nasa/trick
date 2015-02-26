/********************************* TRICK HEADER *******************************
PURPOSE:
    (This routine runs are the end of the targeting iteration sequence on the
     Master side of the simulation.)
REFERENCE:
    (((None)))
ASSUMPTIONS AND LIMITATIONS:
    ((Many.))
CLASS:
    (monte_master_shutdown)
LIBRARY DEPENDENCY:
    ((target_master_shutdown.o))
PROGRAMMERS:
    (((Edwin Z. Crues) (NASA) (Nov 2009) (--) (Initial version.)))
*******************************************************************************/

/* System includes. */

/* Trick includes. */
//#include "sim_services/include/exec_proto.h"
#include "sim_services/MonteCarlo/include/montecarlo_c_intf.h"

/* Model includes */
#include "../include/target_body.h"
#include "trick_utils/comm/include/tc_proto.h"

/* ENTRY POINT */
int target_master_shutdown( TCDevice *tc_dev, TargetBodyData *data, TargetBodyIteration *iterate)
{
   printf("\e[33mtarget_master_shutdown current_run=%d\(\)\e[00m\n",mc_get_current_run());
   /* Return to calling function. */
   return( 0 );

}

