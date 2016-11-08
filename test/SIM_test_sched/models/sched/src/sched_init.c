/*********************************** TRICK HEADER **************************
PURPOSE:                     (Test sched_init)
REFERENCES:                  (none)
ASSUMPTIONS AND LIMITATIONS: (Autocreated by trick_ui)
CLASS:                       (initialization)
LIBRARY DEPENDENCY:          (sched_init.o)
PROGRAMMERS:                 ((vetter) (Tue Aug 26 13:22:44 CDT 2003))
***************************************************************************/

#include "../include/sched.h"

int sched_init(
                            /* RETURN: -- Always return zero */
  SCHEDULE *S)              /* INOUT:  -- Parameter */

{
       S->mass = S->mass*1.0000013 + 0.0000013 ; /* Massive masses */
       return(0) ;
}
