/* PURPOSE:
    (Reset Regula Falsi event time to go estimator after the respective event
     has fired.)

REFERENCE:
    ((See Also: Bob Gottlieb - MDSSC-ES, this code was originally pirated from
      Bob's STAMPS regula_falsi.c routine.))

ASSUMPTIONS AND LIMITATIONS:
    ((The reset_falsi function should be called every time an event is fired.))

PROGRAMMERS:
    (((Robert W. Bailey) (LinCom Corp) (5/15/91) (Trick-CR-00000) (Rob's Whim)))
*/

/* FUNCTION:
    Resets the time to go and dependent variable boundaries like the
    set_falsi function but resets the independent variable boundaries to
    current simulation time.
*/

#include "trick/regula_falsi.h"

void reset_regula_falsi(
                           /* Return: Estimated time (seconds) to go to error
                                      function zero point */
        double time,       /* In: Current time - input (seconds) */
        REGULA_FALSI * R)  /* Inout: Regula Falsi parameters */
{
    R->delta_time = BIG_TGO;
    R->lower_set = 0;
    R->upper_set = 0;
    R->t_lower = time;
    R->t_upper = time;
    R->x_lower = BIG_TGO;
    R->x_upper = BIG_TGO;
    R->iterations = 0;
    R->last_error = 0.0;

    return;
}
