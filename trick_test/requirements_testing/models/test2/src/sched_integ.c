/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Test scheduling )
REFERENCE:                   ( None ) 
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( integration )
LIBRARY DEPENDENCY:          ( sched_integ.o )
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) (Trick tutorial) )
*******************************************************************************/

/*
 * $Log$
 */
 
#include "sim_services/include/integrator.h"
#include "../include/sched.h"

int sched_integ(
                    /* RETURN: -- Always return zero */
  INTEGRATOR  *I ,  /* INOUT:  -- Trick state integration parameters */
  MSD_NUMERIC *N )  /* INOUT:  -- Schedule struct */
{
  I->state[0] = N->pos ;
  I->state[1] = N->vel ;

  I->deriv[I->intermediate_step][0] = N->vel ;
  I->deriv[I->intermediate_step][1] = N->acc ;

  integrate( I ) ;

  N->pos =  I->state_ws[I->intermediate_step][0] ;
  N->vel =  I->state_ws[I->intermediate_step][1] ;

  return( I->intermediate_step );
}
