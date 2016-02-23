
/*************************************************************************
PURPOSE:      (Cannonball Prototypes)
**************************************************************************/

#ifndef CANNON_INTEG_PROTO_H
#define CANNON_INTEG_PROTO_H
#include <stdio.h>
#include "cannon.h"

#ifdef __cplusplus
extern "C" {
#endif

int cannon_integ(CANNON*) ;
int cannon_deriv(CANNON*) ;
int cannon_init(CANNON*) ;
int cannon_default_data(CANNON*) ;

#ifdef __cplusplus
}
#endif

#endif
