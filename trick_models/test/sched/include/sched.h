/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Scheduling test )
REFERENCES:                  ( None )
ASSUMPTIONS AND LIMITATIONS: ( None )
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) )
*******************************************************************************/

/*
 * $Id: sched.h 1104 2010-09-24 21:56:51Z wwoodard $
 */

#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include "sim_services/Integrator/include/regula_falsi.h"
#include "sim_services/Integrator/include/integrator_c_intf.h"

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
