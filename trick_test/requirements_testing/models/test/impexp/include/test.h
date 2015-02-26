/********************************* TRICK HEADER *******************************
PURPOSE:
    (Trick test)
REFERENCES:
    (((Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (Nov 2001)))
ASSUMPTIONS AND LIMITATIONS:
    ((None))
PROGRAMMERS:
    ((Keith Vetter) (LinCom) (November 2001))
*******************************************************************************/

/*
 * $Log: test.h,v $
 * Revision 7.1  2006-06-22 14:14:17-05  lin
 * Bump version number for 07
 *
 * Revision 5.1  2004-08-05 13:06:53-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:21  lin
 * Bump version number for 04
 *
 * Revision 3.3  2002/10/07 15:16:24  lin
 * Add rcs version info to all trick_models files
 *
 */
 
#ifndef _TEST_H_
#define _TEST_H_

typedef struct { /* A ------------------------------------------------*/

  double d ;            /* -- A double */
  int    i ;            /* -- An int */
  int    j ;            /* -- An int */

} A ; /*--------------------------------------------------------------*/

typedef struct { /* B ------------------------------------------------*/

  double d ;            /* -- A double */
  int    i ;            /* -- An int */
  int    j ;            /* -- An int */

} B ; /*--------------------------------------------------------------*/

typedef struct { /* TEST ---------------------------------------------*/

  A   a ;          /*    --   A */
  B   b ;          /*    --   B */

} TEST ; /*------------------------------------------------------------*/

#endif
