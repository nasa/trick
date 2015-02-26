/********************************* TRICK HEADER *******************************
PURPOSE:
    (Ball::state_integ performs the following:
        - loads the position states into the INTEGRATOR state ws arrays
        - loads the velocity states into the INTEGRATOR state derivative ws
        - loads the velocity states into the INTEGRATOR state ws arrays
        - loads the acceleration states into the INTEGRATOR state derivative ws
        - calls the TRICK state integration service
        - unloads the new pos and vel states from the INTEGRATOR ws arrays)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((2 dimensional space)
     (integrate accel to pos as two first order diffEQs))
CLASS:
    (integration)
LIBRARY DEPENDENCY:
    ((BallState_default_data.o))
PROGRAMMERS:
    (((Robert W. Bailey) (Sweet Systems Inc) (March 1997) (Tutorial Lesson 1))
     ((Edwin Z. Crues)(Titan Systems Corp.)(Jan 2002)(Crude C++ translation)))
*******************************************************************************/

/*
 * $Id: BallState_default_data.cpp 3383 2014-01-22 23:18:15Z alin $
 *
 * Revision 5.2  2006-04-13 14:39:41-05  jkowing
 * Export functions to header files.
 *
 * Revision 5.1  2004-08-05 13:06:23-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:42  lin
 * Bump version number for 04
 *
 * Revision 1.2  2002/10/07 15:15:57  lin
 * Add rcs version info to all trick_models files
 *
 */

#include <stdlib.h>

/* Model include files. */
#include "trick_utils/units/include/constant.h"
#include "../include/BallState.hh"
#include "sim_services/MemoryManager/include/memorymanager_c_intf.h"

/* ENTRY POINT */
int BallState::default_data() {

    // This data is the same as in the constructor.
    input.mass = 10.0 ;
    input.speed = 3.5 ;
    input.elevation = 45.0 * DTR ;
    input.position[0] = 5.0 ;
    input.position[1] = 5.0 ;

    return(0);
}
