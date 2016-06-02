
/*************************************************************************
PURPOSE:      (Cannonball Structure)
**************************************************************************/

#ifndef CANNON_H
#define CANNON_H
#include <stdio.h>
#include "sim_services/Integrator/include/regula_falsi.h"
#include "trick_utils/comm/include/tc.h"
#include "trick_utils/comm/include/tc_proto.h"

typedef struct {

        double pos0[2] ;   /* *i m Init position of cannonball */
        double vel0[2] ;   /* *i m Init velocity of cannonball */
        double acc0[2] ;   /* *i m Init acceleration of cannonball */

        double pos[2] ;   /*  m xy-position */
        double vel[2] ;   /*  m/s xy-velocity */
        double acc[2] ;   /*  m/s2 xy-acceleration  */
        double init_speed ; /* *i m/s Init barrel speed */
        double init_angle ; /* *i rad Angle of cannon */

        /* Impact */
        REGULA_FALSI rf ; /* -- Dynamic event  params for impact */
        int impact ;      /* -- Has impact occured? */

        FILE *fp ;        /* -- file pointer */

        /* Communication Connection */
        TCDevice connection ;  /* -- Socket connection for sending position */
} CANNON ;

#endif
