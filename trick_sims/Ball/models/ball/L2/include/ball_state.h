/********************************* TRICK HEADER *******************************
PURPOSE:
    (Ball model EOM state parameter definition.)
REFERENCES:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((2 dimensional space)
     (Translational EOM only)
     (4 events for contact with floor ceiling and 2 walls))
PROGRAMMERS:
    (((Your name) (Your Company) (Date) (Trick Tutorial)))
*******************************************************************************/

#ifndef BALL_STATE_H
#define BALL_STATE_H

#include "sim_services/Integrator/include/regula_falsi.h"

typedef struct { /* BSTATE_IN ------------------------------------------------*/

  /*=== Initial Ball States ===*/
  double mass ;            /* kg  Total mass */
  double position[2] ;     /* m   X(horizontal),Y(vertical) position */
  double speed ;           /* m/s Linear speed */
  double elevation ;       /* rad   Trajectory angle with respect
                                     to the horizontal */
  /* DYNAMIC EVENT INPUTS */
  double floor_y_pos ;     /* m  Horizontal floor location on Y axis */
  double right_wall_x_pos ;/* m  Vertical right wall location on X axis */
  double ceiling_y_pos ;   /* m  Horizontal ceiling location on Y axis */
  double left_wall_x_pos ; /* m  Vertical left wall location on X axis */
  REGULA_FALSI floor ;     /* -- Dynamic event params for floor impact */
  REGULA_FALSI right_wall ;/* -- Dynamic event params for right wall impact*/
  REGULA_FALSI ceiling ;   /* -- Dynamic event params for ceiling impact */
  REGULA_FALSI left_wall ; /* -- Dynamic event params for left wall impact */

} BSTATE_IN ; /*--------------------------------------------------------------*/

typedef struct { /* BSTATE_OUT -----------------------------------------------*/

  double position[2] ;       /* m    X(horizontal), Y(vertical) position */
  double Frequency ;               /* (Hz)  Total mass */
  double velocity[2] ;       /* m/s  X,Y velocity */
  double acceleration[2] ;   /* m/s2 X,Y acceleration */
  double external_force[2] ; /* N    Total external force on ball */

} BSTATE_OUT ; /*-------------------------------------------------------------*/

typedef struct { /* BSTATE_WORK ----------------------------------------------*/

  void ** external_force ;   /* ** N    External forces, from 'collect' */

} BSTATE_WORK ; /*------------------------------------------------------------*/

typedef struct { /* BSTATE ---------------------------------------------------*/

  BSTATE_IN   input ;           /*    --   User inputs */
  BSTATE_OUT  output ;          /*    --   User outputs */
  BSTATE_WORK work ;         /*    --   EOM workspace */

} BSTATE ; /*-----------------------------------------------------------------*/

#endif
