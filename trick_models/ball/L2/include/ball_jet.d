/********************************* TRICK HEADER *******************************
PURPOSE:
    (Ball model reaction control jet parameter default initialization data.)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((2 jets)
     (jet 0 fires down)
     (jet 1 fires up))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

/*
 * $Log: ball_jet.d,v $
 * Revision 5.1  2004-08-05 13:06:27-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:37  lin
 * Bump version number for 04
 *
 * Revision 3.2  2002/10/07 15:16:03  lin
 * Add rcs version info to all trick_models files
 *
 */

BJET.input.jet_fail[0] = No , No ;
BJET.input.force[0] {N} = -1.0 , 1.0 ;

