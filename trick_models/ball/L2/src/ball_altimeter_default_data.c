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
#include "ball/L2/include/ball_altimeter.h"

     /* ENTRY POINT */
int ball_altimeter_default_data(
                /* RETURN: -- Always return zero */
  BALT *A ) /* INOUT:  -- Ball EOM state parameters */
{

    A->input.add_noise = Yes ;
    A->input.add_bias  = Yes ;
    A->input.noise  = 0.01 ;
    A->input.bias   = 0.01 ;

    /* RETURN */
    return( 0 ) ;
}

