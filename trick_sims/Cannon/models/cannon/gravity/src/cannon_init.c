/******************************* TRICK HEADER ****************************
PURPOSE: (Set the initial data values)
*************************************************************************/

/* Model Include files */
#include <math.h>
#include "../include/cannon.h"
#include "trick/Flag.h"

/* default data job */
int cannon_default_data( CANNON* C ) {

    C->acc[0] = 0.0;
    C->acc[1] = -9.81;
    C->init_angle = M_PI/6 ;
    C->init_speed  = 50.0 ;
    C->pos0[0] = 0.0 ;
    C->pos0[1] = 0.0 ;

    C->time = 0.0 ;

    C->impact = 0 ;
    C->impactTime = 0.0 ;

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

    return(0) ;
}

/* initialization job */
int cannon_init( CANNON* C) {

    C->vel0[0] = C->init_speed*cos(C->init_angle);
    C->vel0[1] = C->init_speed*sin(C->init_angle);

    C->vel[0] = C->vel0[0] ; 
    C->vel[1] = C->vel0[1] ; 

    C->impactTime = 0.0;
    C->impact = 0.0;

    return 0 ; 
}

