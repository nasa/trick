CANNON.pos[0] = 0.0 ;
CANNON.pos[1] = 0.0 ;
CANNON.acc[0] = 0.0 ;
CANNON.acc[1] = -9.81 ;
CANNON.init_angle {d} = 30.0 ;
CANNON.init_speed  = 50.0 ;

/*
 * Regula falsi dynamic event impact setup
 */
#define BIG_TGO 10000
CANNON.rf.lower_set = No ;
CANNON.rf.upper_set = No ;
CANNON.rf.iterations = 0 ;
CANNON.rf.fires = 0 ;
CANNON.rf.x_lower = BIG_TGO ;
CANNON.rf.t_lower = BIG_TGO ;
CANNON.rf.x_upper = BIG_TGO ;
CANNON.rf.t_upper = BIG_TGO ;
CANNON.rf.delta_time = BIG_TGO ;
CANNON.rf.error_tol = 1.0e-9 ;
CANNON.rf.mode = Decreasing ;

CANNON.impact = No ;
