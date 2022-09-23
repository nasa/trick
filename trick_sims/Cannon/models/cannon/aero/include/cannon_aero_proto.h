/****************************** TRICK HEADER ******************************
PURPOSE: (Cannon_aero Prototypes)
Tutorial Sections 8, and 9
***************************************************************************/
#ifndef CANNON_AERO_PROTO_H
#define CANNON_AERO_PROTO_H
#include "cannon_aero.h"

#ifdef __cplusplus
extern "C" {
#endif

int cannon_init_aero(CANNON_AERO*) ;
int cannon_force_gravity(CANNON_AERO*) ;
int cannon_force_drag(CANNON_AERO*) ;
int cannon_force_lift(CANNON_AERO*) ;
int cannon_force_cross(CANNON_AERO*) ;
int cannon_collect_forces(CANNON_AERO*) ;
int cannon_integ_aero(CANNON_AERO*) ;
double cannon_impact_aero(CANNON_AERO*) ;
int cannon_aero_default_data(CANNON_AERO*) ;

#ifdef __cplusplus
}
#endif
#endif
