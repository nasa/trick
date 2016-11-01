
/*************************************************************************
PURPOSE: ( Cannonball EulerCromer Model )
**************************************************************************/

#ifndef CANNON_EULERCROMER_H
#define CANNON_EULERCROMER_H

#include "cannon_common.h"

#ifdef __cplusplus
extern "C" {
#endif
int cannon_deriv(CANNON*) ;
int cannon_ec_integ(CANNON*) ;
#ifdef __cplusplus
}
#endif

#endif
