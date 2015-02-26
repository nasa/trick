/*********************************** TRICK HEADER **************************
PURPOSE:                     (Get slave sim's evaluation of x)
***************************************************************************/
#include "cannon/aero/include/cannon_aero.h"
#include "sim_services/MonteCarlo/include/montecarlo_c_intf.h"
#include "trick_utils/comm/include/tc_proto.h"

int cannon_post_slave(
                            /* RETURN: -- Always return zero */
  CANNON_AERO* C)           /* INOUT:  -- Parameter */

{

     /* Send slave results */
     tc_write( mc_get_connection_device(), (char*) C, sizeof(CANNON_AERO) );
     return(0) ;
}
