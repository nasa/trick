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
    ((import_slave.o))
PROGRAMMERS:
    ((Keith Vetter) (LinCom) (November 2001)) 
*******************************************************************************/

     /* GLOBAL DATA STRUCTURE DECLARATIONS */
#include <stdio.h>
#include "../include/test.h"

     /* ENTRY POINT */
void import_slave(
                  /* RETURN: -- NA */
  TEST *T )       /* INOUT:  -- TEST */
{

   /* Make a saw tooth */
   if ( T->a.i > 20 ) {
      T->a.i = 0 ;
      T->a.d = 0 ;
  
      T->b.i = 0 ;
      T->b.d = 0 ;
   }

}
