/********************************* TRICK HEADER *******************************
PURPOSE:
    (ball_force_field performs the following:
        - computes a relative vector from the ball to the force field origin
        - computes the unit vector in the direction of this relative vector
        - scales the unit vector by the magnitude of the constant force field)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((2 dimensional space)
     (X-axis is horizontal and positive to the right)
     (Y-axis is vertical and positive up)
     (resulting force is 'collect'ed in the S_define file))
CLASS:
    (derivative)
LIBRARY DEPENDENCY:
    ((ball_force_field.o))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

     /* SYSTEM INCLUDE FILES */
#include <math.h>

     /* GLOBAL DATA STRUCTURE DECLARATIONS */
#include "ball/L1/include/ball_force.h"
#include "ball/L1/include/ball_proto.h"


     /* ENTRY POINT */
int ball_force_field(
                /* RETURN: -- Always return zero */
  BFORCE *F ,   /* INOUT:  -- Ball ext. force parameters */
  double *pos ) /* IN:     M  Current ball position */
{

    /* GET SHORTHAND NOTATION FOR DATA STRUCTURES */
    BFORCE_IN  * FI = &(F->input) ;
    BFORCE_OUT * FO = &(F->output) ;

    /* LOCAL VARIABLE DECLARATIONS */
    double mag ;
    double rel[2] ;
    double unit[2] ;

    /* GET RELATIVE VECTOR FROM BALL TO FORCE ORIGIN */
    rel[0] = FI->origin[0] - pos[0] ;
    rel[1] = FI->origin[1] - pos[1] ;

    /* GET UNIT VECTOR AND POSITION MAGNITUDE FROM BALL TO FORCE ORIGIN */
    mag = sqrt( rel[0]*rel[0] + rel[1]*rel[1] ) ;
    unit[0] = rel[0] / mag ;
    unit[1] = rel[1] / mag ;

    /* COMPUTE EXTERNAL FORCE ON BALL IN THE DIRECTION OF THE UNIT VECTOR */
    FO->force[0] = FI->force * unit[0] ;
    FO->force[1] = FI->force * unit[1] ;

    /* RETURN */
    return(0);
}

