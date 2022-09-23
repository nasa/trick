
/************************TRICK HEADER*************************
PURPOSE:
    (This Trick header comment lists out simulation model file dependencies that are not c++.
     The model files are complete relative path to the "C" file and not the object file as
     in the source code comments.  Multiple trick comments with multiple sets of
     LIBRARY DEPENDENCIES are allowed in the S_define file.)
LIBRARY DEPENDENCIES:
    (
     (ball/L1/src/ball_force_default_data.c)
     (ball/L1/src/ball_force_field.c)
     (ball/L1/src/ball_print.c)
     (ball/L1/src/ball_state_deriv.c)
     (ball/L1/src/ball_state_init.c)
     (ball/L1/src/ball_state_integ.c)
     (ball/L1/src/ball_state_alt_integ.c)
     (ball/L1/src/ball_state_default_data.c)
    )
DEFAULT DATA:
    (
    )
*************************************************************/

#ifndef BALL_PROTO_H
#define BALL_PROTO_H

#include "ball/L1/include/ball_state.h"
#include "ball/L1/include/ball_force.h"

#ifdef __cplusplus
extern "C" {
#endif

int ball_force_default_data(BFORCE*) ;
int ball_force_field(BFORCE*,double*) ;
//int ball_malf( BFORCE *F , double *pos ) ;
//int ball_malf_trigger( double curr_time ) ;
int ball_print(BSTATE*) ;
int ball_state_default_data(BSTATE*) ;
int ball_state_deriv(BSTATE*) ;
int ball_state_init(BSTATE*) ;
int ball_state_integ(BSTATE*) ;
int ball_state_alt_integ(BSTATE*) ;

#ifdef __cplusplus
}
#endif

#endif

