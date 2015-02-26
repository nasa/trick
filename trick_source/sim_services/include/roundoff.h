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
    (floating point number round off macro.)
REFERENCE:
    ((None))
ASSUMPTIONS AND LIMITATIONS:
    ()
PROGRAMMERS:
    (((Robert W. Bailey) (LinCom Corp) (4/92) (--) (Realtime)))
*/

/*
 * $Id: roundoff.h 646 2010-04-28 21:50:31Z lin $
 */

#ifndef _ROUNDOFF_H_
#define _ROUNDOFF_H_

#define TRICK_ROUNDOFF_MAX 1.e15

#define ROUNDOFF(nval,res,val)\
{\
   double fraction , real , sval , my_remainder ;\
   if( val < (TRICK_ROUNDOFF_MAX*res) ) { \
      real = (double)((long)val) ;\
      fraction = val - real ;\
      sval = fraction / res ;\
      my_remainder = sval - (double)((long)sval) ;\
      if ( val > 0 ) {\
         if( my_remainder <= 0.5 ) {\
            nval = real + (double)((long)sval)*res ;\
         }\
         else {\
            nval = real + (double)((long)sval+1)*res ;\
         }\
      }\
      else {\
         if( my_remainder >= -0.5 ) {\
            nval = real + (double)((long)sval)*res ;\
         }\
         else {\
            nval = real + (double)((long)sval-1)*res ;\
         }\
      }\
   }\
   else { \
      nval = val;\
   }\
}

#endif
