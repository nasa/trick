/*
 * $Log: sched.d,v $
 * Revision 5.1  2004-08-05 13:06:56-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:17  lin
 * Bump version number for 04
 *
 * Revision 1.4  2003/08/26 20:24:13  vetter
 * Add Dynamic Event Class To SIM_test_sched
 *
 * Revision 1.3  2002/10/07 15:16:28  lin
 * Add rcs version info to all trick_models files
 *
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
