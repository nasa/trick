
/*************************************************************************
PURPOSE:      (Cannonball Prototypes)
**************************************************************************/

#ifndef CANNON_COMM_PROTO_H
#define CANNON_COMM_PROTO_H
#include <stdio.h>
#include "cannon.h"

#ifdef __cplusplus
extern "C" {
#endif

int cannon_deriv_impact(CANNON*) ;
int cannon_integ(CANNON*) ;
double cannon_impact(CANNON*) ;
int cannon_init(CANNON*) ;
int cannon_default_data(CANNON*) ;
int cannon_init_comm(CANNON*) ;
int cannon_send_position(CANNON*) ;

#ifdef __cplusplus
}
#endif

#endif
