/********************************* TRICK HEADER *******************************
PURPOSE:
    (Ball model external force parameter default init. data.)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((2 dimensional space)
     (Constant Force))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

/*
 * $Log: ball_force.d,v $
 * Revision 5.1  2004-08-05 13:06:24-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:39  lin
 * Bump version number for 04
 *
 * Revision 3.2  2002/10/07 15:15:57  lin
 * Add rcs version info to all trick_models files
 *
 */

BFORCE.input.origin {m} = {0.0, 2.0} ;
BFORCE.input.force {N} = 8.0 ;

