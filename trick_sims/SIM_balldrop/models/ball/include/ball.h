/*************************************************************************
PURPOSE: (Represent the state and initial conditions of a NYE ball drop)
**************************************************************************/
#ifndef BALL_H
#define BALL_H

typedef struct {

    double gravity ;    /* *i m/s2 Acceleration due to gravity */
    double height ;    /* *i m Init Y position of NYE ball */

    double vel ;     /* m/s Current velocity of NYE ball */
    double acc ;     /* m/s2 Current acceleration of NYE ball */
    double pos ;     /* m Current Y position of NYE ball */

    double time;        /* s Model time */
    
    int impact ;        /* -- Has impact occurred? */
    double impactTime;  /* s Time of Impact */

    int gravity_change_command ;    /* m/s2 Acceleration change due to gravity */

} BALL ;

#ifdef __cplusplus
extern "C" {
#endif
    int ball_default_data(BALL*) ;
    int ball_init(BALL*) ;
    int ball_shutdown(BALL*) ;
#ifdef __cplusplus
}
#endif

#endif
