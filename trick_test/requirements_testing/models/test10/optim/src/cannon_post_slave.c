/*********************************** TRICK HEADER **************************
PURPOSE:                     (Get slave sim's evaluation of x)
***************************************************************************/
#include "../../aero/include/cannon_aero.h"
#include "sim_services/include/executive.h"
#include "sim_services/include/exec_proto.h"

int cannon_post_slave(
                            /* RETURN: -- Always return zero */
  CANNON_AERO* C)           /* INOUT:  -- Parameter */

{
        EXECUTIVE* E ;
        E = exec_get_exec();

        /* Send slave results */
        tc_write( &E->monte.work.data_conn, (char*) C, sizeof(CANNON_AERO) );

        return(0) ; 
}
