/******************************* TRICK HEADER ****************************
PURPOSE: (Set the default data values)
LIBRARY_DEPENDENCY: ((cannon_default_data.o))
*************************************************************************/

/* Model Include files */
#include "../include/cannon.h"
#include "../include/cannon_integ_proto.h"
#include "sim_services/include/Flag.h"

/* Entry Point */
int cannon_default_data(CANNON* C)
{
    const double PI = 3.141592 ; 

    C->pos[0] = 0.0 ;
    C->pos[1] = 0.0 ;
    C->acc[0] = 0.0 ;
    C->acc[1] = -9.81 ;
    C->init_angle = PI/6 ;
    C->init_speed  = 50.0 ;

    /*
     * Regula falsi dynamic event impact setup
     */
#define MY_BIG_TGO 10000
    C->rf.lower_set = No ;
    C->rf.upper_set = No ;
    C->rf.iterations = 0 ;
    C->rf.fires = 0 ;
    C->rf.x_lower = MY_BIG_TGO ;
    C->rf.t_lower = MY_BIG_TGO ;
    C->rf.x_upper = MY_BIG_TGO ;
    C->rf.t_upper = MY_BIG_TGO ;
    C->rf.delta_time = MY_BIG_TGO ;
    C->rf.error_tol = 1.0e-9 ;
    C->rf.mode = Decreasing ;

    C->impact = No ;

    return(0) ;
}
