
#ifndef SIMTIME_PROTO_H
#define SIMTIME_PROTO_H

#include "trick/time_offset.h"

#ifdef __cplusplus
extern "C" {
#endif

double get_rettime(void) ;
double get_gmttime(void) ;
double get_mettime(void) ;

GMTTIME * get_rettime_struct(void) ;
GMTTIME * get_gmttime_struct(void) ;
GMTTIME * get_mettime_struct(void) ;

#ifdef __cplusplus
}
#endif
#endif

