/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Test scheduling )
REFERENCE:                   ( None ) 
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( scheduled )
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) )
*******************************************************************************/

/*
 * $Log: sched_scheduled.c,v $
 * Revision 5.1  2004-08-05 13:07:00-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:16  lin
 * Bump version number for 04
 *
 * Revision 1.3  2002/10/07 15:16:33  lin
 * Add rcs version info to all trick_models files
 *
 */
 
#include "../include/sched.h"


int sched_scheduled2(
                  /* RETURN: -- Always return zero */
  SCHEDULE *S )      /* INOUT:  -- Schedule struct */
{
   S->mass = S->mass*1.000007 + 0.000007 ; /* Sensor is collecting dust */

   return( 0 );
}
