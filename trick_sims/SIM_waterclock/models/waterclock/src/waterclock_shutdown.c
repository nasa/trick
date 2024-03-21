/************************************************************************
PURPOSE: (Shutdown the sim)
*************************************************************************/
#include <stdio.h>
#include "../include/waterclock.h"
#include "trick/exec_proto.h"

int waterclock_shutdown( WATERCLOCK* WC) {
    double t = exec_get_sim_time();
    printf( "========================================\n");
    printf( "      Water Clock Shutdown     \n");
    printf( "========================================\n");
    return 0 ;
}
