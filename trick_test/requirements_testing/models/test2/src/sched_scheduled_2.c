/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Test scheduling )
REFERENCE:                   ( None ) 
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( scheduled )
LIBRARY DEPENDENCY:          ( sched_scheduled_2.o )
*******************************************************************************/

/*
 * $Log$
 */
 
#include "../include/sched.h"
#include <math.h>

int sched_scheduled_2(
   MSD *M )

{
   M->delta_pos = M->analytic.pos - M->numeric.pos ;
   M->delta_vel = M->analytic.vel - M->numeric.vel;

   return( 0 );
}
