/********************************* TRICK HEADER *******************************
PURPOSE:
    (Ball model altitude control system parameter definition.)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((2 jets)
     (jet 0 fires down)
     (jet 1 fires up)
     (simple bang-bang phase plane))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

#ifndef BALL_CONTROL_H
#define BALL_CONTROL_H

#include "sim_services/include/Flag.h"

typedef struct { /* BCONTROL_IN ----------------------------------------------*/

  Flag   active ;          /* --  Yes = control system active */
  double target_altitude ; /* M   Altitude to achieve and hold */
  double dead_zone ;       /* M   delta altitude about target altitude where
                                  no jets are fired */
  double max_rate ;        /* M/s Maximum allowed altitude rate of change */

} BCONTROL_IN ; /*------------------------------------------------------------*/

typedef struct { /* BCONTROL_OUT ---------------------------------------------*/

  Flag jet_command[2] ;    /* --  Reaction control jet commands */

} BCONTROL_OUT ; /*-----------------------------------------------------------*/

typedef struct { /* BCONTROL_WORK --------------------------------------------*/

  double last_altitude ;   /* *io M  Sensed altitude from last pass */
  double rate_est ;        /* M/s Estimated altitude rate of change */

} BCONTROL_WORK ; /*----------------------------------------------------------*/

typedef struct { /* BCONTROL -------------------------------------------------*/

  BCONTROL_IN   input ;       /*    --   User inputs */
  BCONTROL_OUT  output ;      /*    --   User outputs */
  BCONTROL_WORK work ;     /*    --   Model workspace */

} BCONTROL ; /*---------------------------------------------------------------*/

#endif


