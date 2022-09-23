/****************************** TRICK HEADER ******************************
PURPOSE: (Drag force)
Tutorial Section 8
***************************************************************************/
#include "../include/cannon_aero_proto.h"
#include "trick_utils/math/include/trick_math.h"

int cannon_force_drag( CANNON_AERO *C ) {

        double k ;
        double speed ;

        speed = V_MAG( C->vel ) ;

        /* k = -1/2*rho*Cd*A*|V| */
        k = (-0.5)*C->air_density*C->coefficient_drag*C->ball_area*speed ;

        /* Force_drag = k*V = -1/2*rho*Cd*A*|V|*V */
        V_SCALE( C->force_drag, C->vel, k ) ;

        C->mag_force_drag = V_MAG( C->force_drag ) ;

        return 0 ;
}
