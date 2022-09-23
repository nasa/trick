/*********************************** TRICK HEADER **************************
PURPOSE:                     (Read slave's sim evaluation)
LIBRARY DEPENDENCY:          ((amoeba.o))
PROGRAMMER:                  ((keith))
***************************************************************************/
#include <stdio.h>
#include "cannon/aero/include/cannon_aero.h"
#include "../include/amoeba.h"
#include "../include/amoeba_proto.h"
#include "sim_services/MonteCarlo/include/montecarlo_c_intf.h"
#include "trick_utils/comm/include/tc_proto.h"

int cannon_post_master(
  CANNON_AERO* C,
  AMOEBA* A )
{
        CANNON_AERO C_curr ;

        // remove warning about unused parameter C
        (void)C ;

        /* Read slave's results */
        mc_read( (char*) &C_curr, sizeof(CANNON_AERO) );

        fprintf(stderr, "%03d> F(", mc_get_current_run());
        amoeba_print_point(4, A->curr_point) ;
        fprintf(stderr, "= %.6lf\n", C_curr.pos[0]);

        /*
         * Load function result for either:
         * simplex vertice, centroid, reflection, contraction or expansion point
         */
        A->curr_point[A->num_dims] = C_curr.pos[0] ;

        return(0) ;
}
