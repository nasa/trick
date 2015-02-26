/******************************* TRICK HEADER *******************************
PURPOSE:                     (Test regula falsi. This dynamic event function fires
                              when S->pos passes through 0. )
REFERENCE:                   ( None )
ASSUMPTIONS AND LIMITATIONS: ( )
CLASS:                       (dynamic_event)
LIBRARY DEPENDENCY:          (sched_dyn_event.o)
*****************************************************************************/

/*
 * $Log$
 */

#include <stdio.h>
#include "../include/sched.h"
#include "sim_services/include/regula_falsi.h"
#include "sim_services/include/exec_proto.h"

double sched_dyn_event(
                          /* RETURN: s  Time to go to event */
MSD_NUMERIC * N ,            /* INOUT:  --  */
double *time ,            /* IN:     s  Integrator time from the executive */
int *event_evaluate_tgo ) /* IN:     -- Event time to go function eval flag,
                                        1 = evaluate time to go for event,
                                        0 = fire event */
{

   double tgo ;

   if( *event_evaluate_tgo ) {

      /* We're finding the root of the position function. */
       N->rf.error = N->pos;
       tgo = regula_falsi( *time , &(N->rf) ) ;

   } else {

       /* FIRE EVENT */
       reset_regula_falsi( *time , &(N->rf) ) ;
       tgo = 0.0 ;
   
       send_hs(stderr,"************************************\n");
       send_hs(stderr,"** Dynamic event fired at %g\n",*time);
       send_hs(stderr,"************************************\n");

   }

   return( tgo ) ;
}
