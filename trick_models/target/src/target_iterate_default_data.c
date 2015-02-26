/********************************* TRICK HEADER *******************************
PURPOSE:
    (Target body iteration parameter default initialization data.)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((Keplerian dynamics)
     (Simplified state transition matrix.))
PROGRAMMERS:
   (((Edwin Z. Crues) (NASA JSC ER7) (Nov 2009) (--) (Tareget body init.)))
*******************************************************************************/
#include "../include/target_body.h"

int target_iterate_default_data(TargetBodyIteration* iterate_data) {
    /* Set the default itertion variables. */
    iterate_data->iter_max      = 100;
    iterate_data->iter_cnt      = 0;
    iterate_data->tolerance     = 1.0e-3;

    /* Clear the computed change in velocity. */
    iterate_data->delta_v[0] = 0.0, 0.0, 0.0;
    return(0);
}

