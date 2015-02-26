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
    (Parameter definitions for the Regula Falsi time-to-go function.)

REFERENCE:
    ((See Also: Bob Gottlieb MDSSC-ES, this software was originally pirated
      from Bob's STAMPS regula_falsi software.))

PROGRAMMERS:
    (((Robert W. Bailey) (LinCom Corp) (5/15/91) (Trick-CR-00000) (Rob's Whim)))
*/

/*
 * $Id: regula_falsi.h 646 2010-04-28 21:50:31Z lin $
 */

#ifndef REGULA_FALSI_DEFINED
#define REGULA_FALSI_DEFINED

#define BIG_TGO 1000.0          /* Some really big number to return when
                                   zero point is not crossed */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {

    Decreasing = -1,   /* Sets time to go = 0 only 
                          for a decreasing err function */
    Any,               /* Sets time to go = 0, 
                          decreasing/increasing err function */
    Increasing         /* Sets time to go = 0 only for 
                          an increasing err function */
} Regula_Falsi;

typedef struct {

    int lower_set;       /* --  Yes = Lower bounds has been set */
    int upper_set;       /* --  Yes = Upper bounds has been set */
    int iterations;      /* --  Total num of iterations 
                                used to fire event */
    int fires;           /* --  Total num of times event fired */
    double x_lower;      /* --  Lower value from error func */
    double t_lower;      /* s   Time at lower val of err func */
    double x_upper;      /* --  Upper value from err function */
    double t_upper;      /* s   Time at upper value of err func */
    double delta_time;   /* s   Time to go */
    double error;        /* --  New Error value from err func */
    double last_error;   /* --  Last pass error value */
    double last_tgo;     /* s   Last pass Time to go value */
    double error_tol;    /* --  Tolerance on error below 
                                which tgo=0 */
    Regula_Falsi mode;   /* --  Operating mode for regula falsi */
    Regula_Falsi function_slope;  /* --  Operating mode for regula falsi */

} REGULA_FALSI;

double regula_falsi(double my_time, REGULA_FALSI * R);
void reset_regula_falsi(double my_time, REGULA_FALSI * R);

#ifdef __cplusplus
}
#endif
#endif
