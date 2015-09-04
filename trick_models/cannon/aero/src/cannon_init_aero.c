/****************************** TRICK HEADER ******************************
PURPOSE: ( Cannon initialization )
Tutorial Section 8
***************************************************************************/
#include <math.h>
#include "../include/cannon_aero_proto.h"
#include "trick_utils/math/include/trick_math.h"

int cannon_init_aero( CANNON_AERO* C) {

        /* Convert omega from spherical (almost) to rectangular */
        C->omega[0] = C->omega0*sin(M_PI/2.0 - C->phi)*cos(C->theta) ;
        C->omega[1] = C->omega0*sin(M_PI/2.0 - C->phi)*sin(C->theta) ;
        C->omega[2] = C->omega0*cos(M_PI/2.0 - C->phi) ;

        return ( 0 );
}
