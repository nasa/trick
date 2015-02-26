/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Test scheduling )
REFERENCE:                   ( None ) 
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( asynchronous )
LIBRARY DEPENDENCY:          ( sched_async.o )
PROGRAMMERS:                 ( (Penn) (LLL) (10-03-2008) )
*******************************************************************************/

#include "../include/sched.h"
#include <unistd.h>
#include <stdio.h>	

/*
 * Run increasing longer periods of time.
 * First time this function is called, it sleeps for 0 seconds;
 * second time, 1 second; third, 2 seconds and so forth. 
 */

int sched_async (
                  /* RETURN: -- Always return zero */
  SCHEDULE *S )   /* INOUT:  -- Schedule struct */
{

   int i;

   /* Try and hang the sim... */
   for ( i=0; i < S->async_completion_count; i++) {
      /* Sleep for 1 second. */
      usleep(1000000) ;
   }
   S->async_completion_count += 1; 

   return( 0 );
}
