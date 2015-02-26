/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Test scheduling )
REFERENCE:                   ( None ) 
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( effector )
LIBRARY DEPENDENCY:          ( sched_effector.o )
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) )
*******************************************************************************/

/*
 * $Log: sched_effector.c,v $
 * Revision 7.1  2006-06-22 14:14:05-05  lin
 * Bump version number for 07
 *
 * Revision 5.1  2004-08-05 13:06:58-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:14  lin
 * Bump version number for 04
 *
 * Revision 1.4  2002/10/07 15:16:30  lin
 * Add rcs version info to all trick_models files
 *
 */
 
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
