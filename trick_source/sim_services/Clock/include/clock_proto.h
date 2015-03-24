
#ifndef CLOCK_PROTO_H
#define CLOCK_PROTO_H

#ifdef __cplusplus
extern "C" {
#endif

long long clock_time() ;
long long clock_wall_time() ;
long long clock_reset(long long ref) ;
int clock_spin(long long ref) ;
int clock_set_reference(long long ref) ;
double clock_get_rt_clock_ratio() ;
int clock_set_rt_clock_ratio(double in_rt_clock_ratio) ;

#ifdef __cplusplus
}
#endif


#endif

