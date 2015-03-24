/********************************* TRICK HEADER *******************************
PURPOSE:
    (Malfunction the ball sim.  Zero out the external force.)
LIBRARY DEPENDENCY:
    ((ball_malf.o))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

     /* SYSTEM INCLUDE FILES */
#include <math.h>

     /* GLOBAL DATA STRUCTURE DECLARATIONS */
#include "ball/L1/include/ball_force.h"
#include "ball/L1/include/ball_proto.h"

     /* ENTRY POINT */
int ball_malf(
                /* RETURN: -- Always return zero */
  BFORCE *F ,   /* INOUT:  -- Ball ext. force parameters */
  double *pos ) /* IN:     M  Current ball position */
{

    BFORCE_OUT * FO = &(F->output) ;

    /* COMPUTE EXTERNAL FORCE ON BALL IN THE DIRECTION OF THE UNIT VECTOR */
    FO->force[0] = 0 ;
    FO->force[1] = 0 ;

    //fprintf(stderr,"here in ball_malf.c %f\n", exec_get_sim_time()) ;

    /* RETURN */
    return(0);
}

