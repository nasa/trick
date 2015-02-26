/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Test scheduling )
REFERENCE:                   ( None ) 
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( logging )
LIBRARY DEPENDENCY:          ( sched_logging.o )
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) )
*******************************************************************************/

/*
 * $Log: sched_logging.c,v $
 * Revision 5.1  2004-08-05 13:07:00-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:15  lin
 * Bump version number for 04
 *
 * Revision 1.4  2002/10/07 15:16:32  lin
 * Add rcs version info to all trick_models files
 *
 */
 
#include "../include/sched.h"

int sched_logging(
                  /* RETURN: -- Always return zero */
  SCHEDULE *S )      /* INOUT:  -- Schedule struct */
{
   return( 0 );
}
