/*****************************************************************************
PURPOSE:    ( Analytical Cannon )
*****************************************************************************/
#include <stdio.h>
#include "../include/cannon.h"
#include "../include/cannon_analytic_proto.h"

int cannon_analytic(
      CANNON* C )
{
    static double time = 0.0 ;

    C->acc[0] = 0.0 ;
    C->acc[1] = -9.81 ;

    C->vel[0] = C->vel0[0] + C->acc0[0]*time ; 
    C->vel[1] = C->vel0[1] + C->acc0[1]*time ; 

    C->pos[0] = C->pos0[0] + C->vel0[0]*time + (0.5)*C->acc0[0]*time*time ; 
    C->pos[1] = C->pos0[1] + C->vel0[1]*time + (0.5)*C->acc0[1]*time*time ; 

    /*
     * Increment time by the time delta associated with this job
     * Note that the 0.01 matches the frequency of this job
     * as specified in the S_define.
     */
    time += 0.01 ;

    return 0 ; 
}
