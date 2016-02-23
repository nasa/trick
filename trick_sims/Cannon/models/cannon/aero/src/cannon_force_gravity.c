/****************************** TRICK HEADER ******************************
PURPOSE: (Gravitational force on cannonball)
Tutorial Section 8
***************************************************************************/
#include "../include/cannon_aero_proto.h"

int cannon_force_gravity(
        CANNON_AERO *C )
{
         C->force_gravity[0] = 0.0 ;
         C->force_gravity[1] = 0.0 ;
         C->force_gravity[2] = C->mass*C->g ;

         return 0 ;
}

