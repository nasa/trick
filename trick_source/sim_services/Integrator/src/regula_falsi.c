
#include <math.h>

#define M_ABS(x) ((x) < 0 ? -(x) : (x))
#include "trick/regula_falsi.h"

void regula_falsi_set_upper (double time, double error, REGULA_FALSI *R) {
    R->error = error;
    R->x_upper = error;
    R->t_upper = time;
    R->upper_set = 1;
}

void regula_falsi_set_lower (double time, double error, REGULA_FALSI *R) {
    R->error = error;
    R->x_lower = error;
    R->t_lower = time;
    R->lower_set = 1;
}

double regula_falsi(
                           /* Return: Est. time to go to err
                                      function zero point */
        double time,       /* In: Current time - input (seconds) */
        REGULA_FALSI * R)  /* Inout: Regula Falsi parameters */
{
    if (R->iterations > 0 &&
        ((M_ABS(R->error) < R->error_tol) ||
        (M_ABS(R->last_error - R->error) < R->error_tol)))
    {
        if (R->mode == Any) {
            /* The tgo is now zero */
            return (0.0);
        } else if (R->mode == Increasing && R->lower_set == 1) {
            /* The tgo is now zero */
            return (0.0);
        } else if (R->mode == Decreasing && R->upper_set == 1) {
            /* The tgo is now zero */
            return (0.0);
        }
    }
    if (R->error < 0.0) {
        regula_falsi_set_lower(time, R->error, R);
    } else if (R->error > 0.0) {
        regula_falsi_set_upper(time, R->error, R);
    }
    R->iterations++;

    /* Have now got upper and lower bounds of zero point */
    if (R->upper_set == 1 && R->lower_set == 1) {
        /* Calculate time to error function zero point */
        if (M_ABS(R->error) < R->error_tol)
            R->delta_time = 0.0;
        else {
            R->delta_time = -R->error /
                            ((R->x_upper - R->x_lower) / (R->t_upper -
                                                          R->t_lower));
            if (R->iterations > 20)
                R->delta_time = 0.0;
        }

        /* Now check for increasing or decreasing constraint */
        switch (R->mode) {
            case Any:         /* Decreasing or increasing slope */
                R->last_error = R->error;
                R->last_tgo = R->delta_time;
                return (R->delta_time);
            case Increasing: /* Increasing slope */
                if (R->function_slope == Increasing) {
                    R->last_error = R->error;
                    R->last_tgo = R->delta_time;
                    return (R->delta_time);
                } else {
                    R->lower_set = 0;
                    R->function_slope = Increasing;
                }
                break;
            case Decreasing: /* Decreasing slope */
                if (R->function_slope == Decreasing) {
                    R->last_error = R->error;
                    R->last_tgo = R->delta_time;
                    return (R->delta_time);
                } else {
                    R->upper_set = 0;
                    R->function_slope = Decreasing;
                }
                break;
        }
        R->function_slope = Any;
    } else if (R->lower_set == 1) {
        R->function_slope = Increasing;
    } else if (R->upper_set == 1) {
        R->function_slope = Decreasing;
    }

    R->iterations = 0;
    R->last_tgo = BIG_TGO;

    return (BIG_TGO);
}
