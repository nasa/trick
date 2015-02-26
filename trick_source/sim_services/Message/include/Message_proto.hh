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
        (Trick runtime simulation executive parameter definition.) 
    ICG: (No)
    REFERENCE: 
        (((Bailey, R.W, and Paddock, E.J.) (Trick Simulation Environment) 
          (NASA:JSC #37943) 
          (JSC / Engineering Directorate / Automation and Robotics Division)
          (June 1994) (--))) 
    ASSUMPTIONS AND LIMITATIONS:
        ((Only 64 levels of nested input data file inclusion.)) 
    PROGRAMMERS: 
        (((Robert W. Bailey) (LinCom) (4/92) (--) (Realtime)) 
         ((Robert W. Bailey) (LinCom) (6/1/91) (Trick-CR-00000) (Initial Release)))
*/

/* 
 * $Id: Message_proto.hh 3529 2014-05-15 15:04:03Z alin $
 */

#ifndef _MESSAGEPROTO_HH_
#define _MESSAGEPROTO_HH_

#include "sim_services/Message/include/MessageSubscriber.hh"

int message_subscribe( Trick::MessageSubscriber * in_ms ) ;
int message_unsubscribe( Trick::MessageSubscriber * in_ms ) ;

#endif

