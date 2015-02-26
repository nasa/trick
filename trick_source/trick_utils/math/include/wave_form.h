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
PURPOSE:
    (General Wave Form generator data structure definition)

ASSUMPTIONS AND LIMITATIONS:
    ((Ramp)
     (Pulse)
     (Square)
     (Square Doublet)
     (Triangle)
     (Triangle Doublet)
     (Sine)
     (Uniform Noise)
     (Gaussian Noise)
     (User Defined))

PROGRAMMERS:
    (((Robert W. Bailey) (LinCom) (5/92) (--) (New dynamics)))
*/

/*
 * $Id: wave_form.h 927 2010-08-09 15:24:15Z lin $
 */

#ifndef WAVE_FORM_H_DEFINED
#define WAVE_FORM_H_DEFINED

#ifndef FILE
#include <stdio.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {

    Pulse,          /* Finite Square Pulse */
    Ramp,           /* Finite pulsewith ramped startup & shutdown */
    Square,         /* Oscillating square wave */
    Square_Doublet, /* Positive/negative square wave pair */
    Triangle,       /* Oscillating triangle or sawtooth wave */
    Triangle_Doublet,  /* Positive/negative triangle wave pair */
    Sine,           /* Oscillating sine wave */
    Noise,          /* Uniform random noise */
    Gauss_Noise,    /* Gaussian random noise */
    User_Wave       /* User defined wave data time history */
} Wave_Form;


typedef struct {

    Wave_Form type; /* -- Type of wave form */
    double start;   /* s  Start time for wave */
    double stop;    /* s  Stop time for wave */
    double ramp;    /* s  Time to ramp wave output from 0 to 1 
                          and 1 to 0 */
    double pause;   /* s  Pause time between end of first doublet 
                          pulse and start of second doublet pulse */
    double frequency; /* (one/s) Frequency of oscillating waves */
    double phase;     /* r  Phase offset for start of 
                            oscillating waves */

    /* 
     * Uniform and gaussian random noise generator data 
     */
    int init_random;           /* -- Yes = Initialize random 
                                           number generator */
    unsigned long random_seed; /* -- Seed for random noise 
                                     generator */

    /* 
     * Gaussian random noise generator data 
     */
    int sigma_range;        /* -- Range of output in standard 
                                  deviations,
                                  3 = 3sigma, 6 = 6sigma, etc. */

    /* 
     * User defined wave data 
     */
    int init_user_data;     /* -- Yes = Open user defined wave 
                                  form data file */
    char *user_data;        /* -- File name for user defined 
                                  wave form */
    FILE *fp;               /* -- File pointer for user 
                                  defined wave form */
    double next_time;       /* s  Next user time from user_data */
    double next_val;        /* -- Next user wave value from 
                                  user_data */
    double user_time;       /* s  Current user time from 
                                  user_data */
    double user_val;        /* -- Current user wave value from 
                                  user_data */

} WAVE_FORM;

#ifdef __cplusplus
}
#endif
#endif
