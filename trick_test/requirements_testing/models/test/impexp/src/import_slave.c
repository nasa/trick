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

/*
 * $Log: import_slave.c,v $
 * Revision 7.1  2006-06-22 14:14:17-05  lin
 * Bump version number for 07
 *
 * Revision 5.1  2004-08-05 13:06:54-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:21  lin
 * Bump version number for 04
 *
 * Revision 3.2  2002/10/07 15:16:26  lin
 * Add rcs version info to all trick_models files
 *
 */
 
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
