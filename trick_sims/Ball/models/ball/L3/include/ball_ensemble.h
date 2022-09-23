/********************************* TRICK HEADER *******************************
PURPOSE:
    ()
CLASS:
    (integration)
LIBRARY DEPENDENCY:
    ((ball_ensemble_integ.o)
     (ball_ensemble_collision.o))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/
#ifndef _BALL_ENSEMBLE_H_
#define _BALL_ENSEMBLE_H_
#include "../../L2/include/ball_state.h"

#ifdef __cplusplus
extern "C" {
#endif

int ball_ensemble_integ( BSTATE *S[] );
double ball_ensemble_collision( BSTATE * states[2] );

#ifdef __cplusplus
}
#endif

#endif
