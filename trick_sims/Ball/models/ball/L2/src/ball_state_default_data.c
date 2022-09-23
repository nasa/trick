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
#include "ball/L2/include/ball_state.h"

     /* ENTRY POINT */
int ball_state_default_data(
              /* RETURN: -- Always return zero */
  BSTATE *S ) /* INOUT:  -- Ball EOM state parameters */
{

    S->input.mass = 10.0 ;
    S->input.speed = 3.5 ;
    S->input.elevation = (0.0174532925199433) * 45.0;
    S->input.position[0] = 5.0;
    S->input.position[1] = 5.0;
    S->input.floor_y_pos      = -10.0 ;
    S->input.right_wall_x_pos =  10.0 ;
    S->input.ceiling_y_pos    =  10.0 ;
    S->input.left_wall_x_pos  = -10.0 ;

    S->input.floor.lower_set = 0 ;
    S->input.floor.upper_set = 0 ;
    S->input.floor.iterations = 0 ;
    S->input.floor.fires = 0 ;
    S->input.floor.x_lower = BIG_TGO ;
    S->input.floor.t_lower = BIG_TGO ;
    S->input.floor.x_upper = BIG_TGO ;
    S->input.floor.t_upper = BIG_TGO ;
    S->input.floor.delta_time = BIG_TGO ;
    S->input.floor.error_tol = 1.0e-6 ;
    S->input.floor.mode = Decreasing ;

    S->input.right_wall.lower_set = 0 ;
    S->input.right_wall.upper_set = 0 ;
    S->input.right_wall.iterations = 0 ;
    S->input.right_wall.fires = 0 ;
    S->input.right_wall.x_lower = BIG_TGO ;
    S->input.right_wall.t_lower = BIG_TGO ;
    S->input.right_wall.x_upper = BIG_TGO ;
    S->input.right_wall.t_upper = BIG_TGO ;
    S->input.right_wall.delta_time = BIG_TGO ;
    S->input.right_wall.error_tol = 1.0e-6 ;
    S->input.right_wall.mode = Increasing ;

    S->input.ceiling.lower_set = 0 ;
    S->input.ceiling.upper_set = 0 ;
    S->input.ceiling.iterations = 0 ;
    S->input.ceiling.fires = 0 ;
    S->input.ceiling.x_lower = BIG_TGO ;
    S->input.ceiling.t_lower = BIG_TGO ;
    S->input.ceiling.x_upper = BIG_TGO ;
    S->input.ceiling.t_upper = BIG_TGO ;
    S->input.ceiling.delta_time = BIG_TGO ;
    S->input.ceiling.error_tol = 1.0e-6 ;
    S->input.ceiling.mode = Increasing ;

    S->input.left_wall.lower_set = 0 ;
    S->input.left_wall.upper_set = 0 ;
    S->input.left_wall.iterations = 0 ;
    S->input.left_wall.fires = 0 ;
    S->input.left_wall.x_lower = BIG_TGO ;
    S->input.left_wall.t_lower = BIG_TGO ;
    S->input.left_wall.x_upper = BIG_TGO ;
    S->input.left_wall.t_upper = BIG_TGO ;
    S->input.left_wall.delta_time = BIG_TGO ;
    S->input.left_wall.error_tol = 1.0e-6 ;
    S->input.left_wall.mode = Decreasing ;

    /* RETURN */
    return( 0 ) ;
}

