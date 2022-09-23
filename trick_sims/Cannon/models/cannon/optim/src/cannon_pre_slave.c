/*********************************** TRICK HEADER **************************
PURPOSE:                     (Slave optimization)
LIBRARY_DEPENDENCY:          (amoeba.o)
PROGRAMMER:                  (keith)
***************************************************************************/
#include "../include/amoeba_proto.h"
#include "cannon/aero/include/cannon_aero.h"
#include <stdio.h>

int cannon_pre_slave(
                            /* RETURN: -- Always return zero */
  CANNON_AERO* C)           /* INOUT:  -- Parameter */

{
        fprintf(stderr, "SLAVE POINT: (%.6lf   %.6lf   %.6lf   %.6lf)\n",
               C->time_to_fire_jet_1,
               C->time_to_fire_jet_2,
               C->time_to_fire_jet_3,
               C->time_to_fire_jet_4) ;
        return(0) ;
}
