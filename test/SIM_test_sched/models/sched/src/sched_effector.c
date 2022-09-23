/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Test scheduling )
REFERENCE:                   ( None )
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( effector )
LIBRARY DEPENDENCY:          ( sched_effector.o )
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) )
*******************************************************************************/

#include "../include/sched.h"

int sched_effector(
                  /* RETURN: -- Always return zero */
  SCHEDULE *S )      /* INOUT:  -- Schedule struct */
{
   S->mass = S->mass*1.0000002 + 0.00000002 ; /* Sensor is collecting dust */

   if ( S->amf == 1 ) {
      /* Things are good */
      S->amf = 2 ;
   } else {
      /* Things are out of order */
      S->amf = 3 ;
      S->amf_error = 1 ;
   }


   return( 0 );
}
