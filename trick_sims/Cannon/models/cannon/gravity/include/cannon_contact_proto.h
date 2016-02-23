
/*************************************************************************
PURPOSE:      (Cannonball Prototypes)
**************************************************************************/

#ifndef CANNON_CONTACT_PROTO_H
#define CANNON_CONTACT_PROTO_H
#include <stdio.h>
#include "cannon.h"

#ifdef __cplusplus
extern "C" {
#endif

int cannon_integ(CANNON*) ;
int cannon_deriv_impact(CANNON*) ;
double cannon_impact(CANNON*) ;
int cannon_init(CANNON*) ;
int cannon_default_data(CANNON*) ;

#ifdef __cplusplus
}
#endif

#endif
