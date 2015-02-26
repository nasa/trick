/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Test scheduling )
REFERENCE:                   ( None ) 
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( integration )
LIBRARY DEPENDENCY:          ( sched_integ.o )
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) (Trick tutorial) )
*******************************************************************************/

/*
 * $Log: sched_integ.c,v $
 * Revision 7.1  2006-06-22 14:14:07-05  lin
 * Bump version number for 07
 *
 * Revision 5.2  2006-04-13 14:39:48-05  jkowing
 * Export functions to header files.
 *
 * Revision 5.1  2004-08-05 13:07:00-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:15  lin
 * Bump version number for 04
 *
 * Revision 1.3  2002/10/07 15:16:32  lin
 * Add rcs version info to all trick_models files
 *
 */
 
#include "sim_services/include/integrator.h"
#include "../include/sched.h"

int sched_integ(
                  /* RETURN: -- Always return zero */
  INTEGRATOR *I , /* INOUT:  -- Trick state integration parameters */
  SCHEDULE *S )      /* INOUT:  -- Schedule struct */
{
  I->state[0] = S->pos ;
  I->state[1] = S->vel ;

  I->deriv[I->intermediate_step][0] = S->vel ;
  I->deriv[I->intermediate_step][1] = S->acc ;

  integrate( I ) ;

  S->pos =  I->state_ws[I->intermediate_step][0] ;
  S->vel =  I->state_ws[I->intermediate_step][1] ;

  return( I->intermediate_step );
}
