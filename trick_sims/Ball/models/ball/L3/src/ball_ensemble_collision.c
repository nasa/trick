/********************************* TRICK HEADER *******************************
PURPOSE: ()
CLASS:
    (dynamic-event)
LIBRARY DEPENDENCY:
    ((ball_ensemble_collision.o))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*o******************************************************************************/
#include "../include/ball_ensemble.h"

double ball_ceiling(BSTATE*) ;
double ball_floor(BSTATE*) ;
double ball_left_wall(BSTATE*) ;
double ball_right_wall(BSTATE*) ;

double ball_ensemble_collision( BSTATE * states[2] ) {
    double tgo ;
    double event_tgo;
    event_tgo = BIG_TGO;

    int i;
    for (i=0 ; i<2 ; i++) {
        tgo = ball_ceiling( states[i]);
        if (tgo < event_tgo) event_tgo = tgo;
        tgo = ball_floor( states[i] ) ;
        if (tgo < event_tgo) event_tgo = tgo;
        tgo = ball_left_wall( states[i]) ;
        if (tgo < event_tgo) event_tgo = tgo;
        tgo = ball_right_wall( states[i]) ;
        if (tgo < event_tgo) event_tgo = tgo;
    }

    return (event_tgo) ;
}
