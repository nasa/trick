/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Test scheduling )
REFERENCE:                   ( None ) 
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( derivative )
LIBRARY DEPENDENCY:          ( sched_deriv.o )
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) (Trick tutorial) )
*******************************************************************************/

/*
 * $Log: sched_deriv.c,v $
 * Revision 5.1  2004-08-05 13:06:58-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:13  lin
 * Bump version number for 04
 *
 * Revision 1.3  2002/10/07 15:16:30  lin
 * Add rcs version info to all trick_models files
 *
 */
 
#include "sim_services/include/integrator.h"
#include "../include/sched.h"


int sched_deriv(
                  /* RETURN: -- Always return zero */
  SCHEDULE *S )      /* INOUT:  -- Schedule struct */
{
   S->acc = -1 * S->pos * S->mass ;

   return( 0 );
}
