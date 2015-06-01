/*
   PURPOSE: (First order transfer function.)

   PROGRAMMERS: (((Robert W. Bailey) (LinCom Corp) (3/21/91) (Trick-CR-00000) (Development))) */

/* FUNCTION:

   tau*X'(t) + X(t) = Y(t)

   Compute X(t+dt) given: X(t), Y(t), Y(t+dt), tau, dt

   1) Output = in if time constant or delta time are zero. 2) Compute coefficient of transient response. 3) Compute out
   at current time based on initial in, initial out, current in and the transient coefficient -- first order
   differential equation. */

#include "trick/trick_math.h"


    /* TOLERANCE FOR TIME CONSTANT AND DELTA TIME STEP */
#define TOLERANCE    (1.0e-10)

    /* ENTRY POINT: */
void trns_fnct_1o(int num_vars, /* In: Number of variables to send through transfer */
                  double in_0[],        /* Inout: Last pass forcing function input */
                  double in_t[],        /* In: Current forcing function input */
                  double dt,    /* In: Time step for tr function: t = t+dt */
                  double tau[], /* In: Time constant for servo lag, each servo */
                  double out_0[],       /* Inout: Last pass output */
                  double out_t[],       /* Out: Current output */
                  int *init)
{                                      /* Inout: Initialization flag */
    int i;
    double transient;           /* Exponential decay coeff for transient response */
    double tau_dt;              /* Time constant / dt */

    if (*init) {
        *init = 0;
        for (i = 0; i < num_vars; i++) {
            in_0[i] = in_t[i];
            out_0[i] = in_t[i];
        }
    }

    /* Compute output based on general solution of first order diff */
    for (i = 0; i < num_vars; i++) {

        if (tau[i] < TOLERANCE || dt < TOLERANCE) {
            /* Output equals input if dt or tau are zero */
            out_t[i] = in_t[i];
        } else {
            /* Compute exponential decay coefficient */
            transient = exp(-dt / tau[i]);
            tau_dt = tau[i] / dt;

            /* Compute output at t+dt */
            out_t[i] = out_0[i] * transient + in_0[i] * (tau_dt - transient * (1.0 - tau_dt))
                + in_t[i] * ((1.0 - tau_dt) - transient * tau_dt);
        }
        out_0[i] = out_t[i];
        in_0[i] = in_t[i];
    }

    return;
}
