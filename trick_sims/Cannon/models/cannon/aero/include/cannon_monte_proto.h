/*********************************** TRICK HEADER **************************
PURPOSE:                     (Test Baseball)
***************************************************************************/

#ifndef CANNON_MONTE_PROTO_H
#define CANNON_MONTE_PROTO_H

#include "cannon_aero.h"

#ifdef __cplusplus
extern "C" {
#endif

int cannon_force_jet(CANNON_AERO*);
int cannon_init_aero(CANNON_AERO*) ;
int cannon_force_gravity(CANNON_AERO*) ;
int cannon_force_drag(CANNON_AERO*) ;
int cannon_force_lift(CANNON_AERO*) ;
int cannon_force_cross(CANNON_AERO*) ;
int cannon_collect_forces(CANNON_AERO*) ;
int cannon_integ_aero(CANNON_AERO*) ;
double cannon_impact_monte(CANNON_AERO*) ;
int cannon_monte_default_data(CANNON_AERO*) ;
int cannon_jet_control(CANNON_AERO*) ;

#ifdef __cplusplus
}
#endif
#endif
