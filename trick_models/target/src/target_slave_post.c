/********************************* TRICK HEADER *******************************
PURPOSE:
    (This routine computes the change in delta-V required to adjust the
     initial velocity to hit the targeted position.)
REFERENCE:
    (((None)))
ASSUMPTIONS AND LIMITATIONS:
    ((Many.))
CLASS:
    (monte_slave_post)
LIBRARY DEPENDENCY:
    ((target_slave_post.o)
     (target_delta_v.o))
PROGRAMMERS:
    (((Edwin Z. Crues) (NASA) (Nov 2009) (--) (Initial version.)))
*******************************************************************************/

/* System includes. */

/* Trick includes. */
#include "trick_utils/comm/include/tc_proto.h"
#include "sim_services/MonteCarlo/include/montecarlo_c_intf.h"
#include "trick_utils/math/include/vector_macros.h"

/* Model includes */
#include "../include/target_body.h"

/* ENTRY POINT */
int
target_slave_post(                 /* RETURN: -- 0 - OK, 1 - Error.     */
   TCDevice            * tc_dev,   /* INOUT: -- Iteration comm device.  */
   TargetBodyInit      * init,     /* INOUT: -- Target init state data. */
   TargetBodyState     * state,    /* INOUT: -- Target body state data. */
   TargetBodyData      * target,   /* INOUT: -- Targetting data.        */
   TargetBodyIteration * iterate ) /* INOUT: -- Iteration control data. */
{
   int dv_ret;

   /* Copy the initial state. */
   V_COPY( iterate->v_init, init->velocity );

   /* Call the delta-V computation. */
   dv_ret = target_delta_v( state, target, iterate );

   /* Send F(x) - which is in TargetBodyIteration. */ 
   tc_write( mc_get_connection_device(), (char*)iterate, sizeof(TargetBodyIteration) );

   /* Return to calling function. */
   return( dv_ret );
}

