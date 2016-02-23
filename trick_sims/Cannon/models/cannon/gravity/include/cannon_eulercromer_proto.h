
/*************************************************************************
PURPOSE:      (Cannonball Prototypes)
**************************************************************************/

#ifndef CANNON_EULERCROMER_PROTO_H
#define CANNON_EULERCROMER_PROTO_H
#include <stdio.h>
#include "cannon.h"

#ifdef __cplusplus
extern "C" {
#endif

int cannon_ec_integ(CANNON*) ;
int cannon_deriv(CANNON*) ;
int cannon_init(CANNON*) ;
int cannon_default_data(CANNON*) ;

#ifdef __cplusplus
}
#endif

#endif
