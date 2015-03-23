/********************************* TRICK HEADER *******************************
PURPOSE:
    ( Testing )
REFERENCE:
    (((Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((None))
CLASS:
    (scheduled)
LIBRARY DEPENDENCY:
    ((import_master.o))
PROGRAMMERS:
    ((Keith Vetter) (LinCom) (November 2001)) 
*******************************************************************************/

     /* GLOBAL DATA STRUCTURE DECLARATIONS */
#include "../include/test.h"

     /* ENTRY POINT */
void import_master(
                  /* RETURN: -- NA */
  TEST *T )       /* INOUT:  -- TEST */
{
  /* Do nothing */
  T->a.i = T->a.i ;
}
