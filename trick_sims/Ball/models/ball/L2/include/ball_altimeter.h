/********************************* TRICK HEADER *******************************
PURPOSE:
    (Ball model altitude sensing parameter definition.)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((Gaussian psuedo-random noise)
     (Constant bias))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

#ifndef BALL_ALTITUDE_H
#define BALL_ALTITUDE_H

#include "sim_services/include/Flag.h"

typedef struct { /* BALT_IN --------------------------------------------------*/

  Flag   add_noise ; /* --   Yes = Add noise to sensed altitude */
  Flag   add_bias ;  /* --   Yes = Add bias to sensed altitude */
  double noise ;     /* m    1 sigma noise */
  double bias ;      /* m    Measurement bias */

} BALT_IN ; /*----------------------------------------------------------------*/

typedef struct { /* BALT_OUT -------------------------------------------------*/

  double altitude ;  /* m    Sensed altitude */

} BALT_OUT ; /*---------------------------------------------------------------*/

typedef struct { /* BALT -----------------------------------------------------*/

  BALT_IN  input ;      /*    --   User inputs */
  BALT_OUT output ;     /*    --   User outputs */

} BALT ; /*-------------------------------------------------------------------*/

#endif

