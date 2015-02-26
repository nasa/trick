/*******************************************************************************
*                                                                              *
* Trick Simulation Environment Software                                        *
*                                                                              *
* Copyright (c) 1996,1997 LinCom Corporation, Houston, TX                      *
* All rights reserved.                                                         *
*                                                                              *
* Copyrighted by LinCom Corporation and proprietary to it. Any unauthorized    *
* use of Trick Software including source code, object code or executables is   *
* strictly prohibited and LinCom assumes no liability for such actions or      *
* results thereof.                                                             *
*                                                                              *
* Trick Software has been developed under NASA Government Contracts and        *
* access to it may be granted for Government work by the following contact:    *
*                                                                              *
* Contact: Charles Gott, Branch Chief                                          *
*          Simulation and Graphics Branch                                      *
*          Automation, Robotics, & Simulation Division                         *
*          NASA, Johnson Space Center, Houston, TX                             *
*                                                                              *
*******************************************************************************/
/* 
   PURPOSE: (Second order transfer function.)

   ASSUMPTIONS AND LIMITATIONS: ((A linear function input function variation between function calls.))

   PROGRAMMERS: (((Robert W. Bailey) (LinCom Corp) (3/21/91) (Trick-CR-00000) (Development))) */

/* 
 * $Id: trns_fnct_2o.c 49 2009-02-02 22:37:59Z lin $
 */

#include <stdio.h>
#include "../include/trick_math.h"

void trns_fnct_2o(double *z,    /* In: damping ratio */
                  double *wn,   /* In: r/s Natural frequency in */
                  double dt,    /* In: s Time step in unit-time */
                  double *in,   /* In: The current input values */
                  double *inwk, /* Inout: The previous pass values */
                  double *out,  /* Inout: The output values */
                  double *outwk,        /* Inout: Derivative of the output */
                  int nd,       /* In: The number of array elements */
                  int *init)
{                                      /* In: Initialization flag */
    int i;
    double wf, wfdt, c, sdwf, acc;
    double a, a0, a1, a2, a3, a4;


    if (*init) {
        *init = 0;
        for (i = 0; i < nd; i++) {
            outwk[i] = in[i];
            out[i] = in[i];
            inwk[i] = in[i];
        }
    }

    for (i = 0; i < nd; i++) {

        a0 = 2.0 * z[i] / wn[i];
        sdwf = exp(-wn[i] * z[i] * dt);
        wf = wn[i] * sqrt(1 - z[i] * z[i]);
        wfdt = dt * wf;
        c = sdwf * cos(wfdt);
        sdwf *= (sin(wfdt) / wf);
        a = wn[i] * sdwf;

        acc = (in[i] - inwk[i]) / dt;
        a1 = a0 * acc;
        a2 = out[i] - inwk[i] + a1;
        a4 = wn[i] * a2;
        a3 = outwk[i] - acc;

        out[i] = a2 * c + (a4 * z[i] + a3) * sdwf + in[i] - a1;
        outwk[i] = a3 * c - (a4 + a3 * z[i]) * a + acc;
        inwk[i] = in[i];
    }

    return;
}
