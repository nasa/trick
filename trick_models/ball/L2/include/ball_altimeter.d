/********************************* TRICK HEADER *******************************
PURPOSE:
    (Ball model altitude sensor parameter default initialization data.)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((Random Gaussian noise)
     (constant bias))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

/*
 * $Log: ball_altimeter.d,v $
 * Revision 5.1  2004-08-05 13:06:26-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:35  lin
 * Bump version number for 04
 *
 * Revision 3.2  2002/10/07 15:16:01  lin
 * Add rcs version info to all trick_models files
 *
 */

BALT.input.add_noise = Yes ;
BALT.input.add_bias  = Yes ;
BALT.input.noise {M} = 0.01 ;
BALT.input.bias {M}  = 0.01 ;

