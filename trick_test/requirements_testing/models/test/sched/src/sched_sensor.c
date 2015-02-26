/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Test scheduling )
REFERENCE:                   ( None ) 
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( sensor )
LIBRARY DEPENDENCY:          ( sched_sensor.o )
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) )
*******************************************************************************/

/*
 * $Log: sched_sensor.c,v $
 * Revision 7.1  2006-06-22 14:14:08-05  lin
 * Bump version number for 07
 *
 * Revision 5.1  2004-08-05 13:07:01-05  lin
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


int sched_sensor(
                  /* RETURN: -- Always return zero */
  SCHEDULE *S )      /* INOUT:  -- Schedule struct */
{
   S->mass = S->mass*1.000008 + 0.000008 ; /* Sensor is collecting dust */

   return( 0 );
}
