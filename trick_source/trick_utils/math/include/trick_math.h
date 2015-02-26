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
 * $Id: trick_math.h 49 2009-02-02 22:37:59Z lin $
 */

#ifndef _TRICK_MATH_
#define _TRICK_MATH_

/*
 * Function prototypes for all functions in ${TRICK_HOME}/sim_services/math
 */

#include <math.h>

#ifndef M_ABS
#define M_ABS(x) ((x) < 0 ? -(x) : (x))
#endif

#if ( __ghs )
#ifndef M_PI
#define M_PI     3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2          1.570796326794896619231
#endif
#endif

#include "vector_macros.h"
#include "matrix_macros.h"
#include "quat_macros.h"
#include "trick_math_error.h"
#include "trick_math_proto.h"

#endif
