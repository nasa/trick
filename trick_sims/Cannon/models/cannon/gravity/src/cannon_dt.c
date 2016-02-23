/*****************************************************************************
PURPOSE:    ( Try dt )
*****************************************************************************/
#include "../include/cannon.h"

int cannon_dt(
      CANNON* C )
{
        double dt ;
        double pos0[2] ;
        double vel0[2] ;
        double acc0[2] ;

        /* This dt matches the frequency of this job */
        dt = 0.01 ;

        /* Save off last state */
        pos0[0] = C->pos[0] ; pos0[1] = C->pos[1] ;
        vel0[0] = C->vel[0] ; vel0[1] = C->vel[1] ;
        acc0[0] = C->acc[0] ; acc0[1] = C->acc[1] ;

        /* Calculate new state based on last state */
        C->acc[0] = acc0[0] ;
        C->acc[1] = acc0[1] ;

        C->vel[0] = vel0[0] + acc0[0]*dt ; 
        C->vel[1] = vel0[1] + acc0[1]*dt ; 

        C->pos[0] = pos0[0] + vel0[0]*dt + (0.5)*acc0[0]*dt*dt ; 
        C->pos[1] = pos0[1] + vel0[1]*dt + (0.5)*acc0[1]*dt*dt ; 

        return 0 ; 
}
