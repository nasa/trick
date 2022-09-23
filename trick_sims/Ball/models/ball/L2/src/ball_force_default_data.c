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
#include "ball/L1/include/ball_force.h"

     /* ENTRY POINT */
int ball_force_default_data(
              /* RETURN: -- Always return zero */
  BFORCE *F ) /* INOUT:  -- Ball EOM state parameters */
{

    F->input.origin[0] = 0.0 ;
    F->input.origin[1] = 2.0 ;
    F->input.force = 8.0 ;

    /* RETURN */
    return( 0 ) ;
}

