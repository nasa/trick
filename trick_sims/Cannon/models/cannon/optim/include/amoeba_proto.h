/*
 * Vetter's implementation of the Nelder-Mead "amoeba" algorithm.
 * Was made strictly for Trick tutorial.  Was a whim.
 * Reference for algorithm (not code):
 * http://www.research.ibm.com/infoecon/paps/html/amec99_bundle/node8.html
 */

#ifndef AMOEBA_PROTO_H
#define AMOEBA_PROTO_H

#include "amoeba.h"
#include "cannon/aero/include/cannon_aero.h"

#ifdef __cplusplus
extern "C" {
#endif

int cannon_init_amoeba(AMOEBA*);
int cannon_pre_master(CANNON_AERO*, AMOEBA*);
int cannon_post_master(CANNON_AERO*, AMOEBA*);
int cannon_post_slave(CANNON_AERO*);
int cannon_pre_slave(CANNON_AERO*);

#ifdef __cplusplus
}
#endif

#endif
