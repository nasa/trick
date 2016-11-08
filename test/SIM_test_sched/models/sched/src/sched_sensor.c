/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Test scheduling )
REFERENCE:                   ( None )
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( sensor )
LIBRARY DEPENDENCY:          ( sched_sensor.o )
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) )
*******************************************************************************/

#include "../include/sched.h"

int sched_sensor(
                  /* RETURN: -- Always return zero */
  SCHEDULE *S )      /* INOUT:  -- Schedule struct */
{
   S->mass = S->mass*1.000008 + 0.000008 ; /* Sensor is collecting dust */

   return( 0 );
}
