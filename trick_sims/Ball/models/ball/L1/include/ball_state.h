
/** TRICK HEADER **************************************************************
@file

@verbatim
PURPOSE:
    (Ball model EOM state parameter definition.)
ASSUMPTIONS AND LIMITATIONS:
    ((2 dimensional space)
     (Translational EOM only))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
@endverbatim
*******************************************************************************/


#ifndef BALL_STATE_H
#define BALL_STATE_H

/** @struct BSTATE_IN
    @brief ball state input parameters
 */
typedef struct { /* BSTATE_IN ------------------------------------------------*/

  /*=== Initial Ball States ===*/
  double mass ;            /**< (kg)  Total mass */
  double position[2] ;     /**< (m)   X(horizontal),Y(vertical) position */
  double speed ;           /**< (m/s) Linear speed */
  double elevation ;       /**< (rad)   Trajectory angle with respect to the horizontal */
  int print_off;           /**< (--)  Set to nonzero to suppress output */
} BSTATE_IN ; /*--------------------------------------------------------------*/

/** @struct BSTATE_OUT
    @brief ball state output parameters
 */
typedef struct { /* BSTATE_OUT -----------------------------------------------*/

  double position[2] ;       /**< (m)    X(horizontal), Y(vertical) position */
  double velocity[2] ;       /**< (m/s)  X,Y velocity */
  double acceleration[2] ;   /**< (m/s2) X,Y acceleration */
  double external_force[2] ; /**< (N)    Total external force on ball */

} BSTATE_OUT ; /*-------------------------------------------------------------*/

/** @struct BSTATE_WORK
    @brief ball state work parameters
 */
typedef struct { /* BSTATE_WORK ----------------------------------------------*/

  void ** external_force ;   /**< ** (N)    External forces, from 'collect' */

} BSTATE_WORK ; /*------------------------------------------------------------*/

/** @struct BSTATE
    @brief ball state structure
 */
typedef struct { /* BSTATE ---------------------------------------------------*/

  BSTATE_IN   input ;           /**<    (--)   User inputs */
  BSTATE_OUT  output ;          /**<    (--)   User outputs */
  BSTATE_WORK work ;         /**<    (--)   EOM workspace */

} BSTATE ; /*-----------------------------------------------------------------*/

#endif
