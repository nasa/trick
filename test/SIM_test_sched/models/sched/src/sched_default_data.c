/*********************************** TRICK HEADER **************************
PURPOSE:                     (Test sched_init)
REFERENCES:                  (none)
ASSUMPTIONS AND LIMITATIONS: (Autocreated by trick_ui)
CLASS:                       (initialization)
LIBRARY DEPENDENCY:          (sched_init.o)
PROGRAMMERS:                 ((vetter) (Tue Aug 26 13:22:44 CDT 2003))
***************************************************************************/

#include "../include/sched.h"

/* RETURN: -- Always return zero */
/* INOUT:  -- Parameter */
int sched_default_data(SCHEDULE *S)
{
#define BIG_TGO 10000
#define No 0

    S->pos        = 5.0 ;
    S->vel        = 0.0 ;
    S->acc        = 0.0 ;
    S->mass       = 1.0 ;
    S->amf        = 1   ;
    S->amf_error  = 0   ;

    S->rf.lower_set  = No ;
    S->rf.upper_set  = No ;
    S->rf.iterations = 0 ;
    S->rf.fires      = 0 ;
    S->rf.x_lower    = BIG_TGO ;
    S->rf.t_lower    = BIG_TGO ;
    S->rf.x_upper    = BIG_TGO ;
    S->rf.t_upper    = BIG_TGO ;
    S->rf.delta_time = BIG_TGO ;
    S->rf.error_tol  = 1.0e-6 ;
    S->rf.mode       = Increasing ;

    return(0) ;
}
