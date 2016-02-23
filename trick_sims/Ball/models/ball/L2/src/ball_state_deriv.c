/********************************* TRICK HEADER *******************************
PURPOSE:
    (ball_state_deriv performs the following:
        - collects all the external forces on the ball
        - solves the EOM F=ma for accelerations)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((2 dimensional space)
     (X-axis is horizontal and positive to the right)
     (Y-axis is vertical and positive up)
     (derivative of position already exists as velocity vector)
     (collect contains external forces 'collect'ed in the S_define file))
CLASS:
    (derivative)
LIBRARY DEPENDENCY:
    ((ball_state_deriv.o))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

     /* GLOBAL DATA STRUCTURE DECLARATIONS */
#include "../include/ball_state.h"

     /* EXTERNAL MACRO DECLARATIONS */
#include "sim_services/include/collect_macros.h"

     /* ENTRY POINT */
int ball_state_deriv(
              /* RETURN: -- Always return zero */
  BSTATE *S ) /* INOUT:  -- Ball EOM state parameters */
{

    /* GET SHORTHAND NOTATION FOR DATA STRUCTURES */
    BSTATE_IN   *SI = &(S->input) ;
    BSTATE_OUT  *SO = &(S->output) ;
    BSTATE_WORK *SW = &(S->work) ;

    /* LOCAL VARIABLE DECLARATIONS */
    double **collected_forces ;
    int ii ;

    /* COLLECT EXTERNAL FORCES ON THE BALL  --  TRUST US ON THIS ONE */
    collected_forces = (double**)(SW->external_force) ;
    SO->external_force[0] = 0.0 ;
    SO->external_force[1] = 0.0 ;
    for( ii = 0 ; ii < NUM_COLLECT(collected_forces) ; ii++ ) {
        SO->external_force[0] += collected_forces[ii][0] ;
        SO->external_force[1] += collected_forces[ii][1] ;
    }

    /* SOLVE FOR THE X AND Y ACCELERATIONS OF THE BALL */
    SO->acceleration[0] = SO->external_force[0] / SI->mass ;    /* X acceleration */
    SO->acceleration[1] = SO->external_force[1] / SI->mass ;    /* Y acceleration */

    /* RETURN */
    return(0) ;
}

