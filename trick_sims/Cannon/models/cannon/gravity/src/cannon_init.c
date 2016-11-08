/*****************************************************************************
PURPOSE: (Initialize the cannonball)
*****************************************************************************/
#include <math.h>
#include "../include/cannon_common.h"

int cannon_init( CANNON* C) {

        C->vel0[0] = C->init_speed*cos(C->init_angle);
        C->vel0[1] = C->init_speed*sin(C->init_angle);

        C->vel[0] = C->vel0[0] ;
        C->vel[1] = C->vel0[1] ;

        C->impactTime = 0.0;
        C->impact = 0.0;

        return 0 ;
}

