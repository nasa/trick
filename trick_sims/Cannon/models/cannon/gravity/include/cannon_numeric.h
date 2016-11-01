
/*************************************************************************
PURPOSE: ( Cannonball Numeric Model )
**************************************************************************/

#ifndef CANNON_NUMERIC_H
#define CANNON_NUMERIC_H

#include "cannon_common.h"

#ifdef __cplusplus
extern "C" {
#endif
int cannon_integ(CANNON*) ;
int cannon_deriv(CANNON*) ;
double cannon_impact(CANNON*) ;
#ifdef __cplusplus
}
#endif

#endif
