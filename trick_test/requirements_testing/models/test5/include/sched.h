/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Tests requirements: 3.1.2.2, 3.1.2.2.1-7 ) 
REFERENCES:                  ( None )
ASSUMPTIONS AND LIMITATIONS: ( None ) 
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) )
*******************************************************************************/

/*
 * $Log: sched.h,v $
 * Revision 5.1  2004-08-05 13:06:57-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:17  lin
 * Bump version number for 04
 *
 * Revision 1.4  2003/08/26 20:24:14  vetter
 * Add Dynamic Event Class To SIM_test_sched
 *
 * Revision 1.3  2002/10/07 15:16:28  lin
 * Add rcs version info to all trick_models files
 *
 */
 
#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include "sim_services/include/regula_falsi.h"

typedef struct { /* SCHEDULE ------------------------------------------------*/

  double pos ;    /* M    Position */
  double vel ;    /* M/s  Velocity */
  double acc ;    /* M/s2 Acceleration */
  double mass ;   /* kg   Mass */
  int amf     ;   /* --   Just a test variable */
  int amf_error ; /* --  Order error occured in amf job */
  REGULA_FALSI rf ; /* -- Dynamic event */

} SCHEDULE ; /*--------------------------------------------------------------*/


#endif
