/********************************* TRICK HEADER *******************************
PURPOSE:
    (Trigger the ball malfunction.  This example triggers after 30 seconds.)
LIBRARY DEPENDENCY:
    ((ball_malf_trigger.o))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

     /* SYSTEM INCLUDE FILES */
#include <math.h>

     /* GLOBAL DATA STRUCTURE DECLARATIONS */
#include "ball/L1/include/ball_force.h"
#include "sim_services/include/exec_proto.h"
#include "ball/L1/include/ball_proto.h"


     /* ENTRY POINT */
int ball_malf_trigger(
                /* RETURN: -- Always return zero */
  double curr_time )   /* INOUT:  -- Ball ext. force parameters */
{
    return(curr_time > 30.0);
}

