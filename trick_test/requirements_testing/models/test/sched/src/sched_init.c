/*********************************** TRICK HEADER **************************
PURPOSE:                     (Test sched_init)
REFERENCES:                  (none)
ASSUMPTIONS AND LIMITATIONS: (Autocreated by trick_ui)
CLASS:                       (initialization)
LIBRARY DEPENDENCY:          (sched_init.o)
PROGRAMMERS:                 ((vetter) (Tue Aug 26 13:22:44 CDT 2003))
***************************************************************************/

/*
 * $Log: sched_init.c,v $
 * Revision 7.1  2006-06-22 14:14:07-05  lin
 * Bump version number for 07
 *
 * Revision 5.1  2004-08-05 13:06:59-05  lin
 * Bump
 *
 * Revision 4.2  2004/01/16 21:48:56  lin
 * Add a log to all files
 *
 */

#include "../include/sched.h"

int sched_init(
                            /* RETURN: -- Always return zero */
  SCHEDULE *S)              /* INOUT:  -- Parameter */

{
       S->mass = S->mass*1.0000013 + 0.0000013 ; /* Massive masses */
       return(0) ; 
}
