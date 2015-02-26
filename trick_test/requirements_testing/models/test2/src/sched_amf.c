/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Test scheduling )
REFERENCE:                   ( None ) 
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( asynchronous_mustfinish )
LIBRARY DEPENDENCY:          ( sched_amf.o )
*******************************************************************************/
 
#include "../include/sched.h"
#include <unistd.h>
#include <stdio.h>

int sched_amf(
                  /* RETURN: -- Always return zero */
  SCHEDULE *S )      /* INOUT:  -- Schedule struct */
{

   if ((S->amf_completion_count % 2 ) == 0) {
       printf("AMF sleeping 1.5 seconds.\n");
       usleep(1500000);  /* 1.5 seconds */
   } else {
       printf("AMF sleeping 0.5 seconds.\n");
       usleep( 500000);  /* 0.5 seconds */
   }

   S->amf_completion_count ++;

   return( 0 );
}
