/****************************** TRICK HEADER ******************************
PURPOSE: (Lift-Magnus Force)
Tutorial Section 8
***************************************************************************/
#include "../include/cannon_aero_proto.h"
#include "trick_utils/math/include/trick_math.h"

int cannon_force_lift( CANNON_AERO *C ) {

        double w_cross_v[3] ; double norm_w_cross_v[3] ;
        double k, speed ;

        speed = V_MAG( C->vel ) ;

        if ( speed != 0.0 ) {
                C->spin_parameter = C->ball_radius*V_MAG( C->omega )/speed ;
        } else {
                C->spin_parameter = 0.0000000001 ;
        }

        if ( C->lift_method == Smits_Smith ) {
                C->coefficient_lift = (0.54)*pow(C->spin_parameter, 0.4) ;
        }

        if ( C->lift_method == Tombras ) {
                C->coefficient_lift = 1/( 2.022 + 0.981*speed/V_MAG( C->omega));
        }

        switch ( C->lift_method ) {

        case  Hard_Coded_Coefficient_Lift:
        case  Smits_Smith:
        case  Tombras:

                /* k = 1/2*rho*Cl*A*V^2 */
                k = (0.5)*C->air_density*C->coefficient_lift*
                    C->ball_area*speed*speed ;

                /* F = k*(w x V)/|w x V| */
                V_CROSS( w_cross_v, C->omega, C->vel ) ;
                V_NORM( norm_w_cross_v, w_cross_v ) ;
                V_SCALE( C->force_magnus, norm_w_cross_v, k) ;

                break ;

        case Adair_Giordano:

                V_CROSS( w_cross_v, C->omega, C->vel ) ;
                k = 0.00041*C->mass ;
                V_SCALE( C->force_magnus, w_cross_v, k) ;

                /* Backwards calculation for Cl */
                C->coefficient_lift = (2*k*V_MAG(w_cross_v))/
                                     (C->air_density*C->ball_area*speed*speed) ;
                break ;
        }

        C->mag_force_magnus = V_MAG( C->force_magnus ) ;

        return 0 ;
}

