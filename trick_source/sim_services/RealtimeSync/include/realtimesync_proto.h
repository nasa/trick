/*
 * $Id: realtimesync_proto.h 2640 2012-09-28 15:39:38Z alin $
 */

#ifndef _REALTIMESYNC_PROTO_H_
#define _REALTIMESYNC_PROTO_H_

#ifdef __cplusplus
#include "sim_services/Clock/include/Clock.hh"
#include "sim_services/Timer/include/Timer.hh"
int real_time_change_clock(Trick::Clock * in_clock ) ;
int real_time_change_timer(Trick::Timer * in_sleep_timer ) ;
#endif

#ifdef __cplusplus
extern "C" {
#endif

int real_time_enable() ;
int real_time_disable() ;
int real_time_restart(long long ref_time ) ;
int is_real_time() ;
const char * real_time_clock_get_name() ;
int real_time_set_rt_clock_ratio(double in_clock_ratio) ;


#ifdef __cplusplus
}
#endif


#endif

