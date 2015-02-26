/********************************* TRICK HEADER *******************************
PURPOSE:
    (Ball::state_deriv performs the following:
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
    ((BallStateDeriv.o))
PROGRAMMERS:
    (((Robert W. Bailey) (Sweet Systems Inc) (March 1997) (Tutorial Lesson 1))
     ((Edwin Z. Crues)(Titan Systems Corp.)(Jan 2002)(Crude C++ translation)))
*******************************************************************************/

/*
 * $Id: BallStateDeriv.cpp 3383 2014-01-22 23:18:15Z alin $
 *
 * Revision 5.2  2006-05-02 10:13:31-05  lin
 * Remove deprecated C++ code in SIM_Ball++_L1 code
 *
 * Revision 5.1  2004-08-05 13:06:22-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:41  lin
 * Bump version number for 04
 *
 * Revision 1.2  2002/10/07 15:15:56  lin
 * Add rcs version info to all trick_models files
 *
 */

/* System include files. */
#include <stdio.h>
#include <iostream>

/* Trick include files. */
#include "sim_services/include/collect_macros.h"

/* Model include files. */
#include "../include/Ball.hh"

/* ENTRY POINT */
int Ball::state_deriv() /* RETURN: -- Always return zero. */
{

   /* GET SHORTHAND NOTATION FOR DATA STRUCTURES */
   BallStateInput  * state_in   = &(this->state.input);
   BallStateOutput * state_out  = &(this->state.output);
   BallStateWork   * state_work = &(this->state.work);

   /* COLLECT EXTERNAL FORCES ON THE BALL  --  TRUST US ON THIS ONE */
   //collected_forces = (double**)(state_work->external_force) ;
   state_out->external_force[0] = 0.0 ;
   state_out->external_force[1] = 0.0 ;

   unsigned int ii;
   for( ii = 0 ; ii < state_work->num_external_force ; ii++ ) {
      state_out->external_force[0] += state_work->external_force[ii][0];
      state_out->external_force[1] += state_work->external_force[ii][1];
   }

   /* SOLVE FOR THE X AND Y ACCELERATIONS OF THE BALL */
   state_out->acceleration[0] = state_out->external_force[0] / state_in->mass;
   state_out->acceleration[1] = state_out->external_force[1] / state_in->mass;

   /* RETURN */
   return(0);

}

