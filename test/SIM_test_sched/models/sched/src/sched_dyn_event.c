/******************************* TRICK HEADER *******************************
PURPOSE:                     (Test regula falsi )
REFERENCE:                   ( None )
ASSUMPTIONS AND LIMITATIONS: ( A program has to know its limitations,
                               that is my assumption :P )
CLASS:                       (dynamic_event)
LIBRARY DEPENDENCY:          (sched_dyn_event.o)
PROGRAMMERS:                 ( Keith Vetter a.k.a brillo )
*****************************************************************************/

#include <stdio.h>
#include "../include/sched.h"

double sched_dyn_event(       /* RETURN: s  Time to go to event */
    SCHEDULE * S)             /* INOUT:  --  */
{

    double tgo;  /* Estimated time to go (in seconds) to the defined event */
    double now = get_integ_time();

    /* CALCULATE TIME TO GO FOR EVENT */
    S->rf.error = S->mass - 1.2;
    tgo = regula_falsi( now , &(S->rf) );

    if ( tgo == 0.0 ) {
        /* FIRE EVENT */
        reset_regula_falsi( now , &(S->rf) );
        /* Make mass zig zag once it hits 1.2kg */
        S->mass = S->mass - 0.01;
    }

    return( tgo ) ;
}
