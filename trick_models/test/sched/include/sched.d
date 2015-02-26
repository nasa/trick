/*
 * $Id: sched.d 1104 2010-09-24 21:56:51Z wwoodard $
 */

SCHEDULE.pos        = 5.0 ;
SCHEDULE.vel        = 0.0 ;
SCHEDULE.acc        = 0.0 ;
SCHEDULE.mass       = 1.0 ;
SCHEDULE.amf        = 1   ;
SCHEDULE.amf_error  = 0   ;

#define BIG_TGO 10000
SCHEDULE.rf.lower_set  = No ;
SCHEDULE.rf.upper_set  = No ;
SCHEDULE.rf.iterations = 0 ;
SCHEDULE.rf.fires      = 0 ;
SCHEDULE.rf.x_lower    = BIG_TGO ;
SCHEDULE.rf.t_lower    = BIG_TGO ;
SCHEDULE.rf.x_upper    = BIG_TGO ;
SCHEDULE.rf.t_upper    = BIG_TGO ;
SCHEDULE.rf.delta_time = BIG_TGO ;
SCHEDULE.rf.error_tol  = 1.0e-6 ;
SCHEDULE.rf.mode       = Increasing ;
