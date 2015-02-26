/********************************* TRICK HEADER *******************************
PURPOSE:
    (Ball::force_field performs the following:
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
    ((BallForceField.o))
PROGRAMMERS:
    (((Robert W. Bailey) (Sweet Systems Inc) (March 1997) (Tutorial Lesson 1))
     ((Edwin Z. Crues)(Titan Systems Corp.)(Jan 2002)(Crude C++ translation)))
*******************************************************************************/

/*
 * $Id: BallForceField.cpp 627 2010-03-30 16:21:13Z wwoodard $
 *
 * Revision 5.1  2004-08-05 13:06:22-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:41  lin
 * Bump version number for 04
 *
 * Revision 1.2  2002/10/07 15:15:55  lin
 * Add rcs version info to all trick_models files
 *
 */

/* System include files. */
#include <math.h>

/* Model include files. */
#include "../include/Ball.hh"

/* ENTRY POINT */
int Ball::force_field() /* RETURN: -- Always return zero. */
{

   /* LOCAL VARIABLE DECLARATIONS */
   double mag;
   double rel_pos[2];
   double unit[2];

   /* GET SHORTHAND NOTATION FOR DATA STRUCTURES */
   BallForceInput  * force_in  = &(this->force.input);
   BallForceOutput * force_out = &(this->force.output);
   double * ball_pos = this->state.output.position;

   /* GET RELATIVE VECTOR FROM BALL TO FORCE ORIGIN */
   rel_pos[0] = force_in->origin[0] - ball_pos[0];
   rel_pos[1] = force_in->origin[1] - ball_pos[1];

   /* GET UNIT VECTOR AND POSITION MAGNITUDE FROM BALL TO FORCE ORIGIN */
   /* Note sigularity when ball position = force origin. */
   mag = sqrt( rel_pos[0]*rel_pos[0] + rel_pos[1]*rel_pos[1] );
   unit[0] = rel_pos[0] / mag;
   unit[1] = rel_pos[1] / mag;

   /* COMPUTE EXTERNAL FORCE ON BALL IN THE DIRECTION OF THE UNIT VECTOR */
   force_out->force[0] = force_in->force * unit[0];
   force_out->force[1] = force_in->force * unit[1];

   /* RETURN */
   return(0);

}

