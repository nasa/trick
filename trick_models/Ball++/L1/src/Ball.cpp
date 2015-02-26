/********************************* TRICK HEADER *******************************
PURPOSE:
    (Ball::Ball ball object constructor.)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((2 dimensional space)
     (X-axis is horizontal and positive to the right)
     (Y-axis is vertical and positive up)
     (resulting force is 'collect'ed in the S_define file))
CLASS:
    (N/A)
LIBRARY DEPENDENCY:
    ((Ball.o))
PROGRAMMERS:
    (((Robert W. Bailey) (Sweet Systems Inc) (March 1997) (Tutorial Lesson 1))
     ((Edwin Z. Crues)(Titan Systems Corp.)(Jan 2002)(Crude C++ translation)))
*******************************************************************************/

/*
 * $Id: Ball.cpp 3383 2014-01-22 23:18:15Z alin $
 *
 * Revision 5.2  2006-05-02 10:13:31-05  lin
 * Remove deprecated C++ code in SIM_Ball++_L1 code
 *
 * Revision 5.1  2004-08-05 13:06:22-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:40  lin
 * Bump version number for 04
 *
 * Revision 1.2  2002/10/07 15:15:55  lin
 * Add rcs version info to all trick_models files
 *
 */

/* System include files. */
#include <iostream>

/* Model include files. */
#include "../include/Ball.hh"

// Default consructor.
Ball::Ball() /* RETURN: -- None. */
{

   // Print out constructor message.
   //cout << "In Ball constructor." << endl;

}

// Destructor.
Ball::~Ball() /* RETURN: -- None. */
{

   // Print out constructor message.
   //cout << "In Ball destructor." << endl;

}

