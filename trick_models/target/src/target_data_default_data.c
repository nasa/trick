/********************************* TRICK HEADER *******************************
PURPOSE:
    (Target body targeting parameter default initialization data.)
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

int target_data_default_data(TargetBodyData* target_data) {
    int iinc;
    int jinc;
    /* Initialize the state transition matrix to identity. */
    for (iinc = 0 ; iinc < 6 ; iinc++ ) {
        for (jinc = 0 ; jinc < 6 ; jinc++ ) {
            target_data->phi[iinc][jinc] = 0.0;
        }
        target_data->phi[iinc][iinc] = 1.0;
    }
    return(0);
}
