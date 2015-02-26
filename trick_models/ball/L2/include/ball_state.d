/********************************* TRICK HEADER *******************************
PURPOSE:
    (Ball model EOM state parameter default initialization data.)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((2-D Space)
     (Translational EOM only)
     (4 events for contact with floor ceiling and 2 walls))
PROGRAMMERS:
   (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

/*
 * $Log: ball_state.d,v $
 * Revision 5.1  2004-08-05 13:06:28-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:37  lin
 * Bump version number for 04
 *
 * Revision 3.2  2002/10/07 15:16:03  lin
 * Add rcs version info to all trick_models files
 *
 */

BSTATE.input.mass {kg} = 10.0 ;
BSTATE.input.speed {M/s} = 3.5 ;
BSTATE.input.elevation {d} = 45.0 ;
BSTATE.input.position[0] {M} = 5.0 , 5.0 ;

    /* DYNAMIC EVENT INPUTS */
BSTATE.input.floor_y_pos      {M} = -10.0 ;
BSTATE.input.right_wall_x_pos {M} =  10.0 ;
BSTATE.input.ceiling_y_pos    {M} =  10.0 ;
BSTATE.input.left_wall_x_pos  {M} = -10.0 ;

#define BIG_TGO 1000.0

    /* FLOOR CONSTRAINT */
BSTATE.input.floor.lower_set = No ;
BSTATE.input.floor.upper_set = No ;
BSTATE.input.floor.iterations = 0 ;
BSTATE.input.floor.fires = 0 ;
BSTATE.input.floor.x_lower = BIG_TGO ;
BSTATE.input.floor.t_lower = BIG_TGO ;
BSTATE.input.floor.x_upper = BIG_TGO ;
BSTATE.input.floor.t_upper = BIG_TGO ;
BSTATE.input.floor.delta_time = BIG_TGO ;
BSTATE.input.floor.error_tol = 1.0e-6 ;
BSTATE.input.floor.mode = Decreasing ;

    /* RIGHT WALL CONSTRAINT */
BSTATE.input.right_wall.lower_set = No ;
BSTATE.input.right_wall.upper_set = No ;
BSTATE.input.right_wall.iterations = 0 ;
BSTATE.input.right_wall.fires = 0 ;
BSTATE.input.right_wall.x_lower = BIG_TGO ;
BSTATE.input.right_wall.t_lower = BIG_TGO ;
BSTATE.input.right_wall.x_upper = BIG_TGO ;
BSTATE.input.right_wall.t_upper = BIG_TGO ;
BSTATE.input.right_wall.delta_time = BIG_TGO ;
BSTATE.input.right_wall.error_tol = 1.0e-6 ;
BSTATE.input.right_wall.mode = Increasing ;

    /* CEILING CONSTRAINT */
BSTATE.input.ceiling.lower_set = No ;
BSTATE.input.ceiling.upper_set = No ;
BSTATE.input.ceiling.iterations = 0 ;
BSTATE.input.ceiling.fires = 0 ;
BSTATE.input.ceiling.x_lower = BIG_TGO ;
BSTATE.input.ceiling.t_lower = BIG_TGO ;
BSTATE.input.ceiling.x_upper = BIG_TGO ;
BSTATE.input.ceiling.t_upper = BIG_TGO ;
BSTATE.input.ceiling.delta_time = BIG_TGO ;
BSTATE.input.ceiling.error_tol = 1.0e-6 ;
BSTATE.input.ceiling.mode = Increasing ;

    /* LEFT WALL CONSTRAINT */
BSTATE.input.left_wall.lower_set = No ;
BSTATE.input.left_wall.upper_set = No ;
BSTATE.input.left_wall.iterations = 0 ;
BSTATE.input.left_wall.fires = 0 ;
BSTATE.input.left_wall.x_lower = BIG_TGO ;
BSTATE.input.left_wall.t_lower = BIG_TGO ;
BSTATE.input.left_wall.x_upper = BIG_TGO ;
BSTATE.input.left_wall.t_upper = BIG_TGO ;
BSTATE.input.left_wall.delta_time = BIG_TGO ;
BSTATE.input.left_wall.error_tol = 1.0e-6 ;
BSTATE.input.left_wall.mode = Decreasing ;

#undef BIG_TGO
