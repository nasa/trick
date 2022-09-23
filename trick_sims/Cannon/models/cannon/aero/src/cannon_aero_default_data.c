/****************************** TRICK HEADER ******************************
PURPOSE: (Set the default data values)
LIBRARY_DEPENDENCY: ((cannon_aero_default_data.o))
Tutorial Sections 8, and 9
***************************************************************************/
#include "../include/cannon_aero_proto.h"

int cannon_aero_default_data(CANNON_AERO* C) {

    double const newton = 4.44822162 ;

    /* Initialize cannon ball shot */
    C->lift_method = Smits_Smith ;
    C->coefficient_drag = 0.45 ;
    C->coefficient_cross = 0.044 ;
    C->mass = 0.145 ;
    C->air_density = 1.29 ;
    C->ball_radius = 3.63/100 ;
    C->ball_area = 41.59/10000 ;
    C->g = -9.81 ;

    /* Tutorial Section 9 */
    C->force_jet_Z_plus = 1.0 * newton ;

    /* Regula Falsi impact critter setup */
    C->rf.lower_set  = No ;
    C->rf.upper_set  = No ;
    C->rf.iterations = 0 ;
    C->rf.fires      = 0 ;
    C->rf.x_lower    = BIG_TGO ;
    C->rf.t_lower    = BIG_TGO ;
    C->rf.x_upper    = BIG_TGO ;
    C->rf.t_upper    = BIG_TGO ;
    C->rf.delta_time = BIG_TGO ;
    C->rf.error_tol  = 1.0e-9 ;
    C->rf.mode       = Decreasing ;

    return(0) ;

}
