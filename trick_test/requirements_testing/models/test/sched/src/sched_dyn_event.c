/******************************* TRICK HEADER *******************************
PURPOSE:                     (Test regula falsi )
REFERENCE:                   ( None )
ASSUMPTIONS AND LIMITATIONS: ( A program has to know its limitations,
                               that is my assumption :P )
CLASS:                       (dynamic_event)
LIBRARY DEPENDENCY:          (sched_dyn_event.o)
PROGRAMMERS:                 ( Keith Vetter a.k.a brillo )
*****************************************************************************/

/*
 * $Log: sched_dyn_event.c,v $
 * Revision 7.1  2006-06-22 14:14:05-05  lin
 * Bump version number for 07
 *
 * Revision 5.2  2006-04-13 14:39:47-05  jkowing
 * Export functions to header files.
 *
 * Revision 5.1  2004-08-05 13:06:58-05  lin
 * Bump
 *
 * Revision 4.2  2004/01/16 21:48:56  lin
 * Add a log to all files
 *
 */

#include <stdio.h>
#include "../include/sched.h"
#include "sim_services/include/regula_falsi.h"

double sched_dyn_event(
                          /* RETURN: s  Time to go to event */
SCHEDULE * S ,            /* INOUT:  --  */
double *time ,            /* IN:     s  Integrator time from the executive */
int *event_evaluate_tgo ) /* IN:     -- Event time to go function eval flag,
                                        1 = evaluate time to go for event,
                                        0 = fire event */
{

   double tgo ;

   if( *event_evaluate_tgo ) {

      /* CALCULATE TIME TO GO FOR EVENT */
       S->rf.error = S->mass - 1.2 ;
       tgo = regula_falsi( *time , &(S->rf) ) ;

   } else {

       /* FIRE EVENT */
       reset_regula_falsi( *time , &(S->rf) ) ;
       tgo = 0.0 ;
   
       /* Make mass zig zag once it hits 1.2kg */
       S->mass = S->mass - 0.01 ;
   }

   return( tgo ) ;
}
