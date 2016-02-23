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
     (Translational EOM only))
PROGRAMMERS:
   (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

/*
 * $Log: ball_state.d,v $
 * Revision 5.1  2004-08-05 13:06:24-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:40  lin
 * Bump version number for 04
 *
 * Revision 3.2  2002/10/07 15:15:59  lin
 * Add rcs version info to all trick_models files
 *
 */

BSTATE.input.mass {kg} = 10.0 ;
BSTATE.input.speed {m/s} = 3.5 ;
BSTATE.input.elevation {d} = 45.0 ;
BSTATE.input.position {m} = {5.0 , 5.0} ;

