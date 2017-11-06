/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Test scheduling )
REFERENCE:                   ( None )
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( sensor_emitter )
LIBRARY DEPENDENCY:          ( sched_sensor_emitter.o )
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) )
*******************************************************************************/

#include "../include/sched.h"

int sched_sensor_emitter(
                  /* RETURN: -- Always return zero */
  SCHEDULE *S )      /* INOUT:  -- Schedule struct */
{
   S->mass = S->mass*1.000009 + 0.000009 ; /* Sensor is collecting dust */

   return( 0 );
}
