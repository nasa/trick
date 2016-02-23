/****************************** TRICK HEADER ******************************
PURPOSE: (Collect all forces and calculate acceleration)
Tutorial Section 8
***************************************************************************/
#include "../include/cannon_aero_proto.h"
#include "sim_services/include/collect_macros.h"

int cannon_collect_forces( CANNON_AERO *C ) {

        double **collected_forces ;
        int ii ;

        /* Collect external forces on the ball */
        collected_forces = (double**)(C->force_collect) ;
        C->force_total[0] = 0.0 ;
        C->force_total[1] = 0.0 ;
        C->force_total[2] = 0.0 ;
        for( ii = 0 ; ii < NUM_COLLECT(collected_forces) ; ii++ ) {
                C->force_total[0] += collected_forces[ii][0] ;
                C->force_total[1] += collected_forces[ii][1] ;
                C->force_total[2] += collected_forces[ii][2] ;
        }

        /* Solve for xyz acceleration */
        C->acc[0] = C->force_total[0] / C->mass ;
        C->acc[1] = C->force_total[1] / C->mass ;
        C->acc[2] = C->force_total[2] / C->mass ;

        return 0 ;
}

