/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Test scheduling )
REFERENCE:                   ( None )
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( integration )
LIBRARY DEPENDENCY:          ( sched_integ.o )
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) (Trick tutorial) )
*******************************************************************************/

#include <stdio.h>

#include "sim_services/Integrator/include/integrator_c_intf.h"
#include "../include/sched.h"

int sched_integ(  /* RETURN: -- Always return zero */
  SCHEDULE *S )   /* INOUT:  -- Schedule struct */
{

    int ipass;

    /* LOAD THE POSITION AND VELOCITY STATES */
    load_state(
        &S->pos ,
        &S->vel ,
        NULL
    );

    /* LOAD THE POSITION AND VELOCITY STATE DERIVATIVES */
    load_deriv(
        &S->vel ,
        &S->acc ,
        NULL
    );

    /* CALL THE TRICK INTEGRATION SERVICE */
    ipass = integrate();

    /* UNLOAD THE NEW POSITION AND VELOCITY STATES */
    unload_state(
        &S->pos ,
        &S->vel ,
        NULL
    );

    /* RETURN */
    return( ipass );
}
