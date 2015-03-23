#ifndef FRAMELOG_PROTO_H
#define FRAMELOG_PROTO_H


#ifdef __cplusplus
extern "C" {
#endif

int frame_log_on() ;
int frame_log_off() ;
int frame_log_set_max_samples(int num) ;

#ifdef __cplusplus
}
#endif

#endif

