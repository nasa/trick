/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Test scheduling )
REFERENCE:                   ( None ) 
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( environment )
LIBRARY DEPENDENCY:          ( sched_environment.o )
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) )
*******************************************************************************/

/*
 * $Log: sched_environment.c,v $
 * Revision 7.1  2006-06-22 14:14:06-05  lin
 * Bump version number for 07
 *
 * Revision 5.1  2004-08-05 13:06:59-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:15  lin
 * Bump version number for 04
 *
 * Revision 1.3  2002/10/07 15:16:31  lin
 * Add rcs version info to all trick_models files
 *
 */
 
#include "sim_services/include/integrator.h"
#include "../include/sched.h"


int sched_environment(
                  /* RETURN: -- Always return zero */
  SCHEDULE *S )      /* INOUT:  -- Schedule struct */
{
   S->mass = S->mass*1.000005 + 0.000005 ; /* Mass growing, dust collection :-) */

   return( 0 );
}
