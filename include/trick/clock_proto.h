
#ifndef CLOCK_PROTO_H
#define CLOCK_PROTO_H

#ifdef __cplusplus
extern "C" {
#endif

long long clock_time(void) ;
long long clock_wall_time(void) ;
long long clock_reset(long long ref) ;
int clock_spin(long long ref) ;
int clock_set_reference(long long ref) ;
double clock_get_rt_clock_ratio(void) ;
int clock_set_rt_clock_ratio(double in_rt_clock_ratio) ;
unsigned long long clock_tics_per_sec(void) ;

#ifdef __cplusplus
}
#endif


#endif

