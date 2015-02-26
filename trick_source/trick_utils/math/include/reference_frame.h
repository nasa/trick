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
    (Reference frame definition parameter definition)

ASSUMPTIONS AND LIMITATIONS:
    ((Must use three mutualy exculsive angles in Euler rotation sequence))

PROGRAMMERS:
    (((Robert W. Bailey) (LinCom Corp) (10/1/91) (--) (--)))
*/

/*
 * $Id: reference_frame.h 49 2009-02-02 22:37:59Z lin $
 */

#ifndef TRANSFORM_DEFINED
#define TRANSFORM_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {

    Roll_Pitch_Yaw, /* Roll Pitch Yaw sequence */
    Roll_Yaw_Pitch, /* Roll Yaw Pitch sequence */
    Pitch_Yaw_Roll, /* Pitch Yaw Roll sequence */
    Pitch_Roll_Yaw, /* Pitch Roll Yaw sequence */
    Yaw_Roll_Pitch, /* Yaw Roll Pitch sequence */
    Yaw_Pitch_Roll  /* Yaw Pitch Roll sequence */
} Euler_Seq;

typedef struct {

    Euler_Seq euler_sequence;   /* --   Euler rotation sequence */
    double euler_angles[3];     /* (r)  Euler rot angles from 
                                        Frame 1 to Frame 2 */
    double location[3];         /* (m)  Frame 1 origin to Frame 2 
                                        origin vector, referenced 
                                        to Frame 1 */
} TRANSFORM;

#ifdef __cplusplus
}
#endif
#endif
