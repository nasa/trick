
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
PURPOSE: (Define the Error conditions and error messages for the 
          trick math routines)
PROGRAMMERS: (((Alex Lin) (NASA) (8/02) () (--)))
*/

/*
 * $Id: trick_math_error.h 49 2009-02-02 22:37:59Z lin $
 */

#ifndef _TRICK_MATH_ERROR_H_
#define _TRICK_MATH_ERROR_H_

#define TM_SUCCESS     0
#define TM_SING_123_P  1
#define TM_SING_123_N  2
#define TM_SING_132_P  3
#define TM_SING_132_N  4
#define TM_SING_213_P  5
#define TM_SING_213_N  6
#define TM_SING_231_P  7
#define TM_SING_231_N  8
#define TM_SING_312_P  9
#define TM_SING_312_N  10
#define TM_SING_321_P  11
#define TM_SING_321_N  12
#define TM_ANG_NAN     13
#define TM_INV_ROT_SEQ 14
#define TM_DIAG_SMALL  15
#define TM_ZERO_DET    16

void tm_print_error( int error) ;
#endif
