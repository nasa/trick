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

#ifndef _ROUNDOFF_H_
#define _ROUNDOFF_H_


#define ROUNDOFF(roundedVal,inputVal)\
{\
   const double resolution = 1.0e-9 ; \
   double fraction , real , sval , my_remainder ;\
   if( inputVal < (1.0e15*resolution) ) { \
      real = (double)((long)inputVal) ;\
      fraction = inputVal - real ;\
      sval = fraction / resolution ;\
      my_remainder = sval - (double)((long)sval) ;\
      if ( inputVal > 0 ) {\
         if( my_remainder <= 0.5 ) {\
            roundedVal = real + (double)((long)sval)*resolution ;\
         }\
         else {\
            roundedVal = real + (double)((long)sval+1)*resolution ;\
         }\
      }\
      else {\
         if( my_remainder >= -0.5 ) {\
            roundedVal = real + (double)((long)sval)*resolution ;\
         }\
         else {\
            roundedVal = real + (double)((long)sval-1)*resolution ;\
         }\
      }\
   }\
   else { \
      roundedVal = inputVal;\
   }\
}


#endif
