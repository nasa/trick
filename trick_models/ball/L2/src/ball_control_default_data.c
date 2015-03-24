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
#include "ball/L2/include/ball_control.h"
#define FEET_TO_METERS 0.3048
#define INCHES_TO_METERS 0.0254

     /* ENTRY POINT */
int ball_control_default_data(
                /* RETURN: -- Always return zero */
  BCONTROL *C ) /* INOUT:  -- Ball EOM state parameters */
{

    C->input.active = 1;
    C->input.target_altitude = 2.0;
    C->input.dead_zone = 1.0 * FEET_TO_METERS;
    C->input.max_rate = 1.0 * INCHES_TO_METERS;
    /* RETURN */
    return( 0 ) ;
}

