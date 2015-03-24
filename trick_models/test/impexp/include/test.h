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

#ifndef TEST_H
#define TEST_H

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
