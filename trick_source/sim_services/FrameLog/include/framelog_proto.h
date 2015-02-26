#ifndef _FRAMELOG_PROTO_H_
#define _FRAMELOG_PROTO_H_


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

