/*
 * $Id: cannon_contact_proto.h 1496 2011-04-12 12:27:43Z wwoodard $
 */

/*************************************************************************
PURPOSE:      (Cannonball Prototypes)
**************************************************************************/

#ifndef _cannon_proto_h_
#define _cannon_proto_h_
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
