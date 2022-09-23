/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Scheduling test )
REFERENCES:                  ( None )
ASSUMPTIONS AND LIMITATIONS: ( None )
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) )
*******************************************************************************/

#ifndef SCHED_H
#define SCHED_H

#include "sim_services/Integrator/include/regula_falsi.h"
#include "sim_services/Integrator/include/integrator_c_intf.h"

typedef struct { /* SCHEDULE ------------------------------------------------*/

  double pos ;    /* m    Position */
  double vel ;    /* m/s  Velocity */
  double acc ;    /* m/s2 Acceleration */
  double mass ;   /* kg   Mass */
  int amf     ;   /* --   Just a test variable */
  int amf_error ; /* --  Order error occured in amf job */
  REGULA_FALSI rf ; /* -- Dynamic event */

} SCHEDULE ; /*--------------------------------------------------------------*/


#endif
