/************************************************************************
PURPOSE: (Print the final ball state.)
*************************************************************************/
#include <stdio.h>
#include "../include/ball.h"
#include "trick/exec_proto.h"

int ball_shutdown( BALL* B ) {
    double t = exec_get_sim_time();
    printf( "========================================\n");
    printf( "      Ball State at Shutdown     \n");
    printf( "t = %g\n", t);
    printf( "acc = %.9f\n", B->acc);
    printf( "vel = %.9f\n", B->vel);
    printf( "========================================\n");
    return 0 ;
}
