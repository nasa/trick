#ifndef WCS_IN_NCS_REP_H
#define WCS_IN_NCS_REP_H
/*******************************************************************************
*                                                                              *
* Trick Simulation Environment Software                                        *
*                                                                              *
* Copyright (c) 2006 L-3 Communications, Houston, TX                           *
* All rights reserved.                                                         *
*                                                                              *
* Copyrighted by L-3 Communications and proprietary to it. Any unauthorized    *
* use of Trick Software including source code, object code or executables is   *
* strictly prohibited and L-3 assumes no liability for such actions or      *
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
( Functions to convert narrow (char) strings to wide (wchar_t) strings and
  vice-versa, as well as functions to determine the number of characters
  that would result, prior to actually performing a conversion.
)

PROGRAMMERS:
    (((John M. Penn) (L-3 Communications) (08/2006)))
*/

/* $Id: wcs_ext.h 401 2009-11-13 20:21:13Z wwoodard $ */

#ifdef __cplusplus
extern "C" {
#endif

#if __Lynx__
int wcs_to_ncs_len ();
int ncs_to_wcs_len ();
int wcs_to_ncs ();
int ncs_to_wcs();
#else
#include <stddef.h>
#include <wchar.h>
size_t wcs_to_ncs_len (const wchar_t *wcs);
size_t ncs_to_wcs_len (const char *ncs);
size_t wcs_to_ncs (const wchar_t *wcs, char *ncs, size_t ncs_max_len );
size_t ncs_to_wcs(const char *ncs, wchar_t *wcs, size_t w_max_size );
#endif

#ifdef __cplusplus
}
#endif

#endif

