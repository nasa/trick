/*****************************************************************************
PURPOSE: (Initialize the cannonball) 
*****************************************************************************/
#include <stdio.h>
#include <math.h>
#include "../include/cannon.h"
#include "../include/cannon_integ_proto.h"

int cannon_init( CANNON* C) {
   
        C->pos[0] = C->pos0[0] ;
        C->pos[1] = C->pos0[1] ;

        C->vel0[0] = C->init_speed*cos(C->init_angle);
        C->vel0[1] = C->init_speed*sin(C->init_angle);

        C->vel[0] = C->vel0[0] ; 
        C->vel[1] = C->vel0[1] ; 

        C->acc[0] = C->acc0[0] ; 
        C->acc[1] = C->acc0[1] ; 

        C->impactTime = 0;
        C->impact = 0;

        return 0 ; 
}

