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

