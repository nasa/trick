/*********************************** TRICK HEADER **************************
PURPOSE:                     (Read slave's sim evaluation)
LIBRARY DEPENDENCY:          ((amoeba.o))
PROGRAMMER:                  ((keith))
***************************************************************************/
#include <stdio.h>
#include "../../aero/include/cannon_aero.h"
#include "../include/amoeba.h"
#include "sim_services/include/executive.h"
#include "sim_services/include/exec_proto.h"

int cannon_post_master(
  CANNON_AERO* C,
  AMOEBA* A )
{
        CANNON_AERO C_curr ;
        EXECUTIVE* E ;
        E = exec_get_exec();

        /* Read slave's results */
        tc_read( &E->monte.work.data_conn,
                 (char*) &C_curr, sizeof(CANNON_AERO) );

        fprintf(stderr, "%03d> F(", E->monte.work.curr_run);
        amoeba_print_point(4, A->curr_point) ;
        fprintf(stderr, ") = %.6lf\n", C_curr.pos[0]);

        /* 
         * Load function result for either:
         * simplex vertice, centroid, reflection, contraction or expansion point
         */
        A->curr_point[A->num_dims] = C_curr.pos[0] ;

        return(0) ; 
}
