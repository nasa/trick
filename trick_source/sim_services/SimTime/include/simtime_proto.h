
#ifndef _SIMTIME_PROTO_H_
#define _SIMTIME_PROTO_H_

#include "time_offset.h"

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

