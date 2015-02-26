/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Test scheduling )
REFERENCE:                   ( None )
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( environment )
LIBRARY DEPENDENCY:          ( sched_environment.o )
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) )
*******************************************************************************/

/*
 * $Id: sched_environment.c 737 2010-06-14 20:47:08Z wwoodard $
 */

#include "../include/sched.h"


int sched_environment(
                  /* RETURN: -- Always return zero */
  SCHEDULE *S )      /* INOUT:  -- Schedule struct */
{
   S->mass = S->mass*1.000005 + 0.000005 ; /* Mass growing, dust collection :-) */

   return( 0 );
}
