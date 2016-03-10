
#ifndef REALTIMESYNC_PROTO_H
#define REALTIMESYNC_PROTO_H

#ifdef __cplusplus
#include "trick/Clock.hh"
#include "trick/Timer.hh"
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
int real_time_lock_memory(int yes_no) ;

// Deprecated
int exec_set_lock_memory(int yes_no) ;

#ifdef __cplusplus
}
#endif


#endif

