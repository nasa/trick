/********************************* TRICK HEADER *******************************
PURPOSE:
    (Ball model reaction control jet parameter definition.)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((2 jets)
     (jet 0 fires down)
     (jet 1 fires up))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

#ifndef BALL_JET_H
#define BALL_JET_H

#include "sim_services/include/Flag.h"

typedef struct { /* BJET_IN --------------------------------------------------*/

  Flag   jet_fail[2] ; /*  --  Yes = jet will not fire */
  double force[2] ;    /*  N   Resulting force magnitude from each jet */

} BJET_IN ; /*----------------------------------------------------------------*/

typedef struct { /* BJET_OUT -------------------------------------------------*/

  double force[2] ;    /*  N   Resulting X,Y force on ball */

} BJET_OUT ; /*---------------------------------------------------------------*/

typedef struct { /* BJET -----------------------------------------------------*/

  BJET_IN  input ;        /*    --   User inputs */
  BJET_OUT output ;       /*    --   User outputs */

} BJET ; /*-------------------------------------------------------------------*/

#endif

