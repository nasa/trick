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
 * $Log: ball_integ.d,v $
 * Revision 7.2  2006-08-28 09:58:28-05  lin
 * Convert the old style allocation statements we have in included sims
 *
 * Revision 7.1  2006-06-22 14:15:03-05  lin
 * Bump version number for 07
 *
 * Revision 5.1  2004-08-05 13:06:24-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:40  lin
 * Bump version number for 04
 *
 * Revision 3.2  2002/10/07 15:15:58  lin
 * Add rcs version info to all trick_models files
 *
 */

#define NUM_STEP        12  /* use up to 12 intermediate steps:
                               8th order RK Fehlberg */
#define NUM_VARIABLES   4   /* x,y position state and x,y velocity state */

/* Unconstrained arrays must be sized */
INTEGRATOR.state = alloc(NUM_VARIABLES) ;
INTEGRATOR.deriv = alloc(NUM_STEP) ;
INTEGRATOR.state_ws = alloc(NUM_STEP) ;

INTEGRATOR.deriv[0] = alloc(NUM_VARIABLES) ;
INTEGRATOR.deriv[1] = alloc(NUM_VARIABLES) ;
INTEGRATOR.deriv[2] = alloc(NUM_VARIABLES) ;
INTEGRATOR.deriv[3] = alloc(NUM_VARIABLES) ;
INTEGRATOR.deriv[4] = alloc(NUM_VARIABLES) ;
INTEGRATOR.deriv[5] = alloc(NUM_VARIABLES) ;
INTEGRATOR.deriv[6] = alloc(NUM_VARIABLES) ;
INTEGRATOR.deriv[7] = alloc(NUM_VARIABLES) ;
INTEGRATOR.deriv[8] = alloc(NUM_VARIABLES) ;
INTEGRATOR.deriv[9] = alloc(NUM_VARIABLES) ;
INTEGRATOR.deriv[10] = alloc(NUM_VARIABLES) ;
INTEGRATOR.deriv[11] = alloc(NUM_VARIABLES) ;

INTEGRATOR.state_ws[0] = alloc(NUM_VARIABLES) ;
INTEGRATOR.state_ws[1] = alloc(NUM_VARIABLES) ;
INTEGRATOR.state_ws[2] = alloc(NUM_VARIABLES) ;
INTEGRATOR.state_ws[3] = alloc(NUM_VARIABLES) ;
INTEGRATOR.state_ws[4] = alloc(NUM_VARIABLES) ;
INTEGRATOR.state_ws[5] = alloc(NUM_VARIABLES) ;
INTEGRATOR.state_ws[6] = alloc(NUM_VARIABLES) ;
INTEGRATOR.state_ws[7] = alloc(NUM_VARIABLES) ;
INTEGRATOR.state_ws[8] = alloc(NUM_VARIABLES) ;
INTEGRATOR.state_ws[9] = alloc(NUM_VARIABLES) ;
INTEGRATOR.state_ws[10] = alloc(NUM_VARIABLES) ;
INTEGRATOR.state_ws[11] = alloc(NUM_VARIABLES) ;

INTEGRATOR.num_state    = NUM_VARIABLES ;
INTEGRATOR.option       = Runge_Kutta_2 ;    /* 2nd order Runge Kutta */
INTEGRATOR.init         = True ;
INTEGRATOR.first_step_deriv = Yes ;

#undef NUM_STEP
#undef NUM_VARIABLES

