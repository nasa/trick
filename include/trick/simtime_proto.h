
#ifndef SIMTIME_PROTO_H
#define SIMTIME_PROTO_H

#include "trick/time_offset.h"

#ifdef __cplusplus
extern "C" {
#endif

double get_rettime() ;
double get_gmttime() ;
double get_mettime() ;

GMTTIME * get_rettime_struct() ;
GMTTIME * get_gmttime_struct() ;
GMTTIME * get_mettime_struct() ;

#ifdef __cplusplus
}
#endif
#endif

