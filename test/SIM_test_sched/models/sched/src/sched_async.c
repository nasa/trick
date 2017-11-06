/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Test scheduling )
REFERENCE:                   ( None )
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( asynchronous )
LIBRARY DEPENDENCY:          ( sched_async.o )
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) )
*******************************************************************************/

#include "../include/sched.h"

int sched_async (
                  /* RETURN: -- Always return zero */
  SCHEDULE *S )   /* INOUT:  -- Schedule struct */
{

   while ( 1 ) {
      /* Try and hang the sim... */
      usleep(1000000) ;
   }

   return( 0 );
}
