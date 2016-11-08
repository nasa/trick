/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Test scheduling )
REFERENCE:                   ( None )
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( effector_emitter )
LIBRARY DEPENDENCY:          ( sched_effector_emitter.o )
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) )
*******************************************************************************/

#include "../include/sched.h"

int sched_effector_emitter(
                  /* RETURN: -- Always return zero */
  SCHEDULE *S )      /* INOUT:  -- Schedule struct */
{
   S->mass = S->mass*1.000003 + 0.000003 ; /* Sensor is collecting dust */

   return( 0 );
}
