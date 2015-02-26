/********************************* TRICK HEADER *******************************
PURPOSE:
    (Ball model state integrator default initialization data.)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((2 dimensional space))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

/*
 * $Log: sched_integ.d,v $
 * Revision 7.3  2007-11-06 10:39:22-06  hchen
 * Change the way of defining a variable in some .d files related to old style allocation and fix some typos
 *
 * Revision 7.2  2007-10-23 11:53:06-05  hchen
 * Convert the old style allocation statements we have in included sims
 *
 * Revision 7.1  2006-06-22 14:14:11-05  lin
 * Bump version number for 07
 *
 * Revision 5.1  2004-08-05 13:06:57-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:18  lin
 * Bump version number for 04
 *
 * Revision 1.2  2002/10/07 15:16:29  lin
 * Add rcs version info to all trick_models files
 *
 */
 
#define NUM_STEP        12  /* use up to 12 intermediate steps:
                               8th order RK Fehlberg */
#define NUM_VARIABLES   4   /* x,y position state and x,y velocity state */

INTEGRATOR.state = alloc(NUM_VARIABLES) ;
INTEGRATOR.deriv = alloc(NUM_STEP) ;
INTEGRATOR.state_ws = alloc(NUM_STEP) ;

for (int kk = 0 ; kk < NUM_STEP ; kk++ ) {
    INTEGRATOR.deriv[kk] = alloc(NUM_VARIABLES) ;
    INTEGRATOR.state_ws[kk] = alloc(NUM_VARIABLES) ;
}

INTEGRATOR.num_state    = NUM_VARIABLES ;
INTEGRATOR.option       = Runge_Kutta_4 ;    /* 4rth order Runge Kutta */
INTEGRATOR.init         = True ;
INTEGRATOR.first_step_deriv = Yes ;

#undef NUM_STEP
#undef NUM_VARIABLES
