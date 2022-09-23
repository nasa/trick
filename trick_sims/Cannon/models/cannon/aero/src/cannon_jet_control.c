/*********************************** TRICK HEADER **************************
PURPOSE:                     (Controls when jets are fired)
***************************************************************************/
#include "../include/cannon_aero.h"
#include "../include/cannon_monte_proto.h"
#include "sim_services/Executive/include/exec_proto.h"
#include "trick_utils/math/include/trick_math.h"

int cannon_jet_control( CANNON_AERO* C )
{
        double sim_time ;

#define CANNON_EQUALS(X,Y) ( fabs(X  - Y) < 1.0e-9 ) ? 1 : 0

        sim_time = exec_get_sim_time() ;

        if ( CANNON_EQUALS(sim_time, roundoff(0.1, C->time_to_fire_jet_1)) ||
             CANNON_EQUALS(sim_time, roundoff(0.1, C->time_to_fire_jet_2)) ||
             CANNON_EQUALS(sim_time, roundoff(0.1, C->time_to_fire_jet_3)) ||
             CANNON_EQUALS(sim_time, roundoff(0.1, C->time_to_fire_jet_4)) ) {

                C->jet_on = 1 ;

        }
        return(0) ;
}
