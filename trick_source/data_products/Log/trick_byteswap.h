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
     (Byte swapping.)
REFERENCE:
     ((NONE))
ASSUMPTIONS AND LIMITATIONS:
     ((NONE))
PROGRAMMERS:
     (((Jane Falgout) (LinCom) (5/98) (--) (--)))
ICG:
     (No)
*/


/* COPIED FROM ISP and changed names, added float and long swaps */


/* *************************************************************** */
/* *************************************************************** */
/* *************************************************************** */

#ifndef _TRICK_BYTESWAP_H_        
#define _TRICK_BYTESWAP_H_

/* Use the #defines to change the names of the functions
   in this directory.  This is so the names will not
   collide with the original names in the comm directory */

#define trick_byteswap_long_long tbs_long_long
#define trick_byteswap_double tbs_double
#define trick_byteswap_float tbs_float
#define trick_byteswap_long tbs_long
#define trick_byteswap_int tbs_int
#define trick_byteswap_short tbs_short

/* byte order swap routine */

#define TRICK_BIG_ENDIAN    (char)0x00
#define TRICK_LITTLE_ENDIAN (char)0x01

#ifdef __cplusplus
extern "C" {
#endif
long long trick_byteswap_long_long(long long input);
double trick_byteswap_double(double input);
float  trick_byteswap_float(float  input);
long   trick_byteswap_long(long    input);
int    trick_byteswap_int(int    input);
short  trick_byteswap_short(short  input);
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

#endif /* _TRICK_BYTESWAP_H_ */
/* Do NOT add anything below this line */

