/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Test scheduling )
REFERENCE:                   ( None ) 
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( asynchronous_mustfinish )
LIBRARY DEPENDENCY:          ( sched_amf.o )
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) )
*******************************************************************************/

/*
 * $Log: sched_amf.c,v $
 * Revision 7.1  2006-06-22 14:14:04-05  lin
 * Bump version number for 07
 *
 * Revision 5.1  2004-08-05 13:06:57-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:13  lin
 * Bump version number for 04
 *
 * Revision 1.3  2002/10/07 15:16:29  lin
 * Add rcs version info to all trick_models files
 *
 */
 
#include "../include/sched.h"

int sched_amf(
                  /* RETURN: -- Always return zero */
  SCHEDULE *S )      /* INOUT:  -- Schedule struct */
{
   if ( S->amf == 2 ) {
      /* Things are in order */
      S->amf = 1 ; 
   } else {
      /* Things are out of order */
      S->amf = 3 ;
      S->amf_error = 1 ;
   }


   return( 0 );
}
