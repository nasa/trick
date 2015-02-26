/*********************************** TRICK HEADER **************************
PURPOSE:                     ( Gravitational force on cannonball )
***************************************************************************/
#include "../include/cannon_aero.h"

int cannon_force_gravity(
        CANNON_AERO *C )
{
         C->force_gravity[0] = 0.0 ;
         C->force_gravity[1] = 0.0 ;
         C->force_gravity[2] = C->mass*C->g ;

         return 0 ;
}

