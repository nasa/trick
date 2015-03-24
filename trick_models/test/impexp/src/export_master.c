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
    ((export_master.o))
PROGRAMMERS:
    ((Keith Vetter) (LinCom) (November 2001)) 
*******************************************************************************/

     /* GLOBAL DATA STRUCTURE DECLARATIONS */
#include "../include/test.h"

     /* ENTRY POINT */
void export_master(
                  /* RETURN: -- NA */
  TEST *T )       /* INOUT:  -- TEST */
{

   T->a.i++ ;  
   T->a.d += 0.1 ;
  
   T->b.i-- ;
   T->b.d -= 0.1 ;
}
