/****************************** TRICK HEADER ******************************
PURPOSE: ( Jet fire force )
Tutorial Section 9
***************************************************************************/
#include "../include/cannon_jet_proto.h"

int cannon_force_jet( CANNON_AERO *C ) {

        if ( C->jet_on && C->jet_count < 4 ) {
                C->force_jet[2] = C->force_jet_Z_plus ;
                C->jet_count++ ;
                C->jet_on = 0  ;
        } else {
                C->force_jet[2] = 0.0 ;
        }
        return 0 ;
}
