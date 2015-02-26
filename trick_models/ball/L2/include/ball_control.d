/********************************* TRICK HEADER *******************************
PURPOSE:
    (Ball model reaction control jet parameter default initialization data.)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((simple bang-bang phase plane))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

/*
 * $Log: ball_control.d,v $
 * Revision 5.1  2004-08-05 13:06:27-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:36  lin
 * Bump version number for 04
 *
 * Revision 3.2  2002/10/07 15:16:02  lin
 * Add rcs version info to all trick_models files
 *
 */

BCONTROL.input.active = Yes ;
BCONTROL.input.target_altitude {M} = 2.0 ;
BCONTROL.input.dead_zone {ft} = 1.0 ;
BCONTROL.input.max_rate {in/s} = 1.0 ;

/*=== Initialize the control system workspace ===*/
BCONTROL.work.last_altitude {M} = 0.0 ;

