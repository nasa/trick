/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Test scheduling )
REFERENCE:                   ( None ) 
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( scheduled )
LIBRARY DEPENDENCY:          ( sched_scheduled.o )
*******************************************************************************/

/*
 * $Log$
 */
 
#include "../include/sched.h"
#include <math.h>

int sched_scheduled(
   MSD_ANALYTIC *A,
   double       *time )

{
   double t = *time;
   double alpha = - (A->zeta) * (A->omega_n);
   double beta = A->omega_d ;

   A->pos = exp( alpha * t) *
                (A->C1 * cos( beta * t) + A->C2 * sin( beta * t));

   A->vel = alpha * exp( alpha * t) *
                (A->C1 * cos( beta * t) + A->C2 * sin( beta * t)) +
            exp( alpha * t) *
                (A->C2 * beta * cos( beta * t) - A->C1 * beta * sin( beta * t)) ;

   return( 0 );
}
