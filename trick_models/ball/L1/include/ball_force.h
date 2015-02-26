/*
 * $Id: ball_force.h 757 2010-06-22 18:40:57Z wwoodard $
 */

/** TRICK HEADER **************************************************************
@file

@verbatim
PURPOSE:
    (Ball model external force parameter definition.)
ASSUMPTIONS AND LIMITATIONS:
    ((2 dimensional space)
     (Constant force)
     (Always toward a stationary point))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
@endverbatim
*******************************************************************************/


#include <pthread.h>

#ifndef _BALL_FORCE_H_
#define _BALL_FORCE_H_

/** @struct BFORCE_IN
    @brief ball force input parameters
 */
typedef struct { /* BFORCE_IN ------------------------------------------------*/

  double origin[2] ;   /**< (m)    Origin of force center */
  double force ;       /**< (N)    Force magnitude */

} BFORCE_IN ; /*--------------------------------------------------------------*/

/** @struct BFORCE_OUT
    @brief ball force output parameters
 */
typedef struct { /* BFORCE_OUT -----------------------------------------------*/

  double force[2] ;    /**< (N)    Resulting X,Y force on ball */

} BFORCE_OUT ; /*-------------------------------------------------------------*/

/** @struct BFORCE
    @brief ball force parameters
 */
typedef struct { /* BFORCE ---------------------------------------------------*/

  BFORCE_IN   input ;     /**<  (--)   User inputs */
  BFORCE_OUT  output ;    /**<  (--)   User outputs */

} BFORCE ; /*-----------------------------------------------------------------*/

#endif
