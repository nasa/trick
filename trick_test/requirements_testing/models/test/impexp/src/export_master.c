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

/*
 * $Log: export_master.c,v $
 * Revision 7.1  2006-06-22 14:14:15-05  lin
 * Bump version number for 07
 *
 * Revision 5.1  2004-08-05 13:06:53-05  lin
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
