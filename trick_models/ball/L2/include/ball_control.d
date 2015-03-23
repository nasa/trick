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

BCONTROL.input.active = Yes ;
BCONTROL.input.target_altitude {M} = 2.0 ;
BCONTROL.input.dead_zone {ft} = 1.0 ;
BCONTROL.input.max_rate {in/s} = 1.0 ;

/*=== Initialize the control system workspace ===*/
BCONTROL.work.last_altitude {M} = 0.0 ;

