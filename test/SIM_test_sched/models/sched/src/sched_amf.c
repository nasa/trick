/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Test scheduling )
REFERENCE:                   ( None )
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( asynchronous_mustfinish )
LIBRARY DEPENDENCY:          ( sched_amf.o )
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) )
*******************************************************************************/

#include "../include/sched.h"
#include "sim_services/UnitTest/include/trick_tests.h"
#include <stdio.h>

int sched_amf(
                  /* RETURN: -- Always return zero */
  SCHEDULE *S )      /* INOUT:  -- Schedule struct */
{

   char test[16] ;
   static int i = 0;

   if ( S->amf == 2 ) {
      /* Things are in order */
      sprintf(test, "Test %d", i);
      add_test_result("sched_amf: Things are in order", test, "");
      i++;
      S->amf = 1 ;
   } else {
      /* Things are out of order */
      sprintf(test, "Test %d", i);
      add_test_result("sched_amf: Things are NOT in order", test, ". ");
      i++;
      S->amf = 3 ;
      S->amf_error = 1 ;
   }


   return( 0 );
}
