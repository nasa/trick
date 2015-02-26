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

/*
 * $Log: ball_jet.h,v $
 * Revision 5.2  2004-08-18 10:27:01-05  lin
 * Take Out *o Spec In Ball L2 For Checkpoint Reload
 *
 * Revision 5.1  2004/08/05 18:06:28  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:37  lin
 * Bump version number for 04
 *
 * Revision 3.3  2002/10/07 15:16:03  lin
 * Add rcs version info to all trick_models files
 *
 */

#ifndef _BALL_JET_H_
#define _BALL_JET_H_

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

