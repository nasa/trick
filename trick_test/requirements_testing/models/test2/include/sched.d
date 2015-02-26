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
 
SCHEDULE.msd.params.initial_pos   = 5.0 ;
SCHEDULE.msd.params.initial_vel   = 0.0 ;
SCHEDULE.msd.params.mass          = 1.0 ;
SCHEDULE.msd.params.damping_const = 0.3 ;
SCHEDULE.msd.params.spring_const  = 10.0 ;

SCHEDULE.async_completion_count   = 0 ;
SCHEDULE.amf_completion_count     = 0 ;

#define BIG_TGO 10
SCHEDULE.msd.numeric.rf.lower_set  = No ;
SCHEDULE.msd.numeric.rf.upper_set  = No ;
SCHEDULE.msd.numeric.rf.iterations = 0 ;
SCHEDULE.msd.numeric.rf.fires      = 0 ;
SCHEDULE.msd.numeric.rf.x_lower    = BIG_TGO ;
SCHEDULE.msd.numeric.rf.t_lower    = BIG_TGO ;
SCHEDULE.msd.numeric.rf.x_upper    = BIG_TGO ;
SCHEDULE.msd.numeric.rf.t_upper    = BIG_TGO ;
SCHEDULE.msd.numeric.rf.delta_time = BIG_TGO ;
SCHEDULE.msd.numeric.rf.error_tol  = 1.0e-5 ;
SCHEDULE.msd.numeric.rf.mode       = Any ;
