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
    ((export_slave.o))
PROGRAMMERS:
    ((Keith Vetter) (LinCom) (November 2001)) 
*******************************************************************************/

/*
 * $Log: export_slave.c,v $
 * Revision 7.1  2006-06-22 14:14:16-05  lin
 * Bump version number for 07
 *
 * Revision 5.1  2004-08-05 13:06:54-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:20  lin
 * Bump version number for 04
 *
 * Revision 3.2  2002/10/07 15:16:25  lin
 * Add rcs version info to all trick_models files
 *
 */
 
     /* GLOBAL DATA STRUCTURE DECLARATIONS */
#include <stdio.h>
#include "../include/test.h"

     /* ENTRY POINT */
void export_slave(
                  /* RETURN: -- NA */
  TEST *T )       /* INOUT:  -- TEST */
{

  /* Do nothing */
  T->a.i = T->a.i ;
 
}
