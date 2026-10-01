/*********************************** TRICK HEADER **************************
PURPOSE:                     (Get slave sim's evaluation of x)
***************************************************************************/
#include "../include/amoeba_proto.h"
#include "cannon/aero/include/cannon_aero.h"

#include "trick/montecarlo_c_intf.h"
#include "trick/tc_proto.h"

int cannon_post_slave(
                            /* RETURN: -- Always return zero */
  CANNON_AERO* C)           /* INOUT:  -- Parameter */

{

     /* Send slave results */
     mc_write( (char*) C, sizeof(CANNON_AERO) );
     return(0) ;
}
