/****************************** TRICK HEADER ******************************
PURPOSE: (Cross Force or Side Force )
Tutorial Section 8
***************************************************************************/
#include "../include/cannon_aero_proto.h"
#include "trick_utils/math/include/trick_math.h"

int cannon_force_cross( CANNON_AERO *C ) {

        double magnus_cross_drag[3] ;
        double norm_magnus_cross_drag[3] ;
        double k, speed ;

        /* k = 1/2*rho*Cy*A*V^2 */
        speed = V_MAG( C->vel ) ;
        k = (-0.5)*C->air_density*C->coefficient_cross*C->ball_area*speed*speed ;

        /* F = k*(M x D)/|M x D| */
        V_CROSS( magnus_cross_drag, C->force_magnus, C->force_drag ) ;
        V_NORM( norm_magnus_cross_drag, magnus_cross_drag ) ;
        V_SCALE( C->force_cross, norm_magnus_cross_drag, k) ;

        C->mag_force_cross = V_MAG( C->force_cross ) ;

        return 0 ;
}

