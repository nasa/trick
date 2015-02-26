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
PURPOSE: (Byte swapping.)
REFERENCES: (Copied from ISP and changed names, added float and long swaps) 
PROGRAMMERS: (((Jane Falgout) (LinCom) (5/98) (--) (--)))
ICG: (No)
*/

/*
 * $Log: trick_byteswap.h,v $
 * Revision 7.2  2008-05-20 11:33:34-05  lin
 * Remove warnings, part 3
 *
 * Revision 7.1  2006-06-22 14:10:10-05  lin
 * Bump version number for 07
 *
 * Revision 5.2  2005-03-11 09:37:45-06  vetter
 * 8 space indent all
 *
 * Revision 5.1  2004-08-05 13:09:15-05  lin
 * Bump
 *
 * Revision 4.2  2004/01/16 21:31:46  lin
 * Add a log to all files
 *
 */

#ifndef _TRICK_BYTESWAP_H_
#define _TRICK_BYTESWAP_H_

#define TRICK_BIG_ENDIAN    (char)0x00
#define TRICK_LITTLE_ENDIAN (char)0x01

#ifdef __cplusplus
extern "C" {
#endif

double trick_byteswap_double(double input);
float trick_byteswap_float(float input);
long trick_byteswap_long(long input);
int trick_byteswap_int(int input);
short trick_byteswap_short(short input);

#ifdef __cplusplus
}
#endif

#define TRICK_GET_BYTE_ORDER(IND)   \
{                                   \
   union {                          \
      long    l;                    \
      char    c[sizeof(long)];      \
   } un;                            \
   un.l = 1;                        \
   if (un.c[sizeof(long)-1] == 1)   \
      IND = TRICK_BIG_ENDIAN;       \
   else                             \
      IND =  TRICK_LITTLE_ENDIAN;   \
}
#endif
