/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Test scheduling )
REFERENCE:                   ( None )
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( effector_receiver )
LIBRARY DEPENDENCY:          ( sched_effector_receiver.o )
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) )
*******************************************************************************/

#include "../include/sched.h"

int sched_effector_receiver(
                  /* RETURN: -- Always return zero */
  SCHEDULE *S )      /* INOUT:  -- Schedule struct */
{
   S->mass = S->mass*1.000004 + 0.000004 ; /* Sensor is collecting dust */

   return( 0 );
}
