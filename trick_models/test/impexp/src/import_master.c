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

/*
 * $Log: import_master.c,v $
 * Revision 5.1  2004-08-05 13:06:54-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:20  lin
 * Bump version number for 04
 *
 * Revision 3.2  2002/10/07 15:16:26  lin
 * Add rcs version info to all trick_models files
 *
 */
 
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
