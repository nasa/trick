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

/*
 * $Log: ball_altimeter.h,v $
 * Revision 5.2  2004-08-18 10:27:00-05  lin
 * Take Out *o Spec In Ball L2 For Checkpoint Reload
 *
 * Revision 5.1  2004/08/05 18:06:27  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:36  lin
 * Bump version number for 04
 *
 * Revision 3.3  2002/10/07 15:16:01  lin
 * Add rcs version info to all trick_models files
 *
 */

#ifndef _BALL_ALTITUDE_H_
#define _BALL_ALTITUDE_H_

#include "sim_services/include/Flag.h"

typedef struct { /* BALT_IN --------------------------------------------------*/

  Flag   add_noise ; /* --   Yes = Add noise to sensed altitude */
  Flag   add_bias ;  /* --   Yes = Add bias to sensed altitude */
  double noise ;     /* M    1 sigma noise */
  double bias ;      /* M    Measurement bias */

} BALT_IN ; /*----------------------------------------------------------------*/

typedef struct { /* BALT_OUT -------------------------------------------------*/

  double altitude ;  /* M    Sensed altitude */

} BALT_OUT ; /*---------------------------------------------------------------*/

typedef struct { /* BALT -----------------------------------------------------*/

  BALT_IN  input ;      /*    --   User inputs */
  BALT_OUT output ;     /*    --   User outputs */

} BALT ; /*-------------------------------------------------------------------*/

#endif

