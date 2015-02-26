/*********************************** TRICK HEADER **************************
PURPOSE:                     (Test sched_init)
REFERENCES:                  (none)
ASSUMPTIONS AND LIMITATIONS: (Autocreated by trick_ui)
CLASS:                       (initialization)
LIBRARY DEPENDENCY:          (sched_init.o)
PROGRAMMERS:                 ((vetter) (Tue Aug 26 13:22:44 CDT 2003))
***************************************************************************/

/*
 * $Log$
 */

#include "../include/sched.h"
#include "sim_services/include/exec_proto.h"

int sched_init(
                            /* RETURN: -- Always return zero */
  SCHEDULE  *S
)

{
       // Initialize the analytic model.
       S->msd.analytic.pos = S->msd.params.initial_pos;
       S->msd.analytic.vel = S->msd.params.initial_vel;
       S->msd.analytic.omega_n =  sqrt( S->msd.params.spring_const / S->msd.params.mass);
       S->msd.analytic.zeta = S->msd.params.damping_const / (2 * S->msd.params.mass * S->msd.analytic.omega_n );

       if (S->msd.analytic.zeta >= 1) {
          printf("Won't work.\n");
          exec_terminate(__FILE__,"This sim doesn't handle the case where the spring is overdamped.");
       }

       S->msd.analytic.omega_d = S->msd.analytic.omega_n * sqrt(1 - S->msd.analytic.zeta * S->msd.analytic.zeta);
       S->msd.analytic.C1 = S->msd.params.initial_pos;
       S->msd.analytic.C2 = (S->msd.params.initial_vel +
                             S->msd.analytic.zeta *
                             S->msd.analytic.omega_n *
                             S->msd.params.initial_pos) / S->msd.analytic.omega_d;

       // Initialize the numeric model.
       S->msd.numeric.pos = S->msd.params.initial_pos;
       S->msd.numeric.vel = S->msd.params.initial_vel;

       return(0) ; 
}
