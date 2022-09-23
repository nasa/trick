
/*************************************************************************
PURPOSE: (Represent the state and initial conditions of a cannonball)
**************************************************************************/
#ifndef CANNON_H
#define CANNON_H
#include "trick/regula_falsi.h"
#include "trick_utils/comm/include/tc.h"
#include "trick_utils/comm/include/tc_proto.h"

typedef struct {

    double vel0[2] ;    /* *i m Init velocity of cannonball */
    double pos0[2] ;    /* *i m Init position of cannonball */
    double init_speed ; /* *i m/s Init barrel speed */
    double init_angle ; /* *i rad Angle of cannon */
    double g ;          /* *i m/s2 acceleration of gravity  */

    double acc[2] ;     /* m/s2 xy-acceleration  */
    double vel[2] ;     /* m/s xy-velocity */
    double pos[2] ;     /* m xy-position */

    double time;        /* s Model time */
    double timeRate;    /* -- Model time per Sim time. */
    /* =1.0 when cannon ball in flight. =0.0 otherwise. */

    int impact ;        /* -- Has impact occured? */
    double impactTime;  /* s Time of Impact */

    /* Impact */
    REGULA_FALSI rf ; /* -- Dynamic event  params for impact */

    /* Communication Connection */
    TCDevice connection ;  /* -- Socket connection for sending position */

} CANNON ;

#ifdef __cplusplus
extern "C" {
#endif
    int cannon_default_data(CANNON*) ;
    int cannon_init(CANNON*) ;
#ifdef __cplusplus
}
#endif

#endif
