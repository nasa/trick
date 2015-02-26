/* Initialize cannon ball shot */
CANNON_AERO.lift_method = Smits_Smith ;
CANNON_AERO.coefficient_drag = 0.45 ;
CANNON_AERO.coefficient_cross = 0.044 ;
CANNON_AERO.mass = 0.145 ;
CANNON_AERO.air_density = 1.29 ;
CANNON_AERO.ball_radius {cm} = 3.63 ;
CANNON_AERO.ball_area {cm2} = 41.59 ;
CANNON_AERO.g = -9.81 ;
CANNON_AERO.force_jet_Z_plus {lbf} = 2.0 ;

/* Regula Falsi impact critter setup */
#define BIG_TGO 10000                                                                              
CANNON_AERO.rf.lower_set  = No ;
CANNON_AERO.rf.upper_set  = No ;
CANNON_AERO.rf.iterations = 0 ;
CANNON_AERO.rf.fires      = 0 ;
CANNON_AERO.rf.x_lower    = BIG_TGO ;
CANNON_AERO.rf.t_lower    = BIG_TGO ;
CANNON_AERO.rf.x_upper    = BIG_TGO ;
CANNON_AERO.rf.t_upper    = BIG_TGO ;
CANNON_AERO.rf.delta_time = BIG_TGO ;
CANNON_AERO.rf.error_tol  = 1.0e-9 ;
CANNON_AERO.rf.mode       = Decreasing ;
