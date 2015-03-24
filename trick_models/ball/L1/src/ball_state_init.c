/********************************* TRICK HEADER *******************************
PURPOSE:
    (ball_state_init performs the following:
        - passes the input position vector to the output vector
        - xforms the input ball speed and trajectory elevation into a
          velocity vector.)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((2 dimensional space)
     (Positive X is horizontal to the right)
     (Positive Y is vertical and up))
CLASS:
    (initialization)
LIBRARY DEPENDENCY:
    ((ball_state_init.o))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

     /* SYSTEM INCLUDE FILES */
#include <math.h>

     /* GLOBAL DATA STRUCTURE DECLARATIONS */
#include "ball/L1/include/ball_state.h"
#include "ball/L1/include/ball_proto.h"

     /* ENTRY POINT */
int ball_state_init(
              /* RETURN: -- Always return zero */
  BSTATE *S ) /* INOUT:  -- Ball EOM state parameters */
{

    /* GET SHORHAND NOTATION FOR DATA STRUCTURES */
    BSTATE_IN  *BI = &(S->input) ;
    BSTATE_OUT *BO = &(S->output) ;

    /* TRANSFER INPUT POSITION STATES TO OUTPUT POSITION STATES */
    BO->position[0] = BI->position[0] ;     /* X state */
    BO->position[1] = BI->position[1] ;     /* Y state */

    /* TRANSFER INPUT SPEED AND ELEVATION INTO THE VELOCITY VECTOR */
    BO->velocity[0] = BI->speed * cos( BI->elevation ) ;       /* X velocity */
    BO->velocity[1] = BI->speed * sin( BI->elevation ) ;       /* Y velocity */

    /* RETURN */
    return( 0 ) ;
}

