/********************************* TRICK HEADER *******************************
PURPOSE:
    (Ball model altitude sensor job.)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((2-Dimensional space)
     (Y-axis is vertical and positive up))
CLASS:
    (sensor)
LIBRARY DEPENDENCY:
    ((ball_altimeter.o))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

/*
 * $Log: ball_altimeter.c,v $
 * Revision 5.2  2005-01-31 15:45:46-06  lin
 * remove "register" keyword
 *
 * Revision 5.1  2004-08-05 13:06:28-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:33  lin
 * Bump version number for 04
 *
 * Revision 3.3  2002/10/07 15:16:04  lin
 * Add rcs version info to all trick_models files
 *
 */

    /*=== Prototype for drandom_gaussian() ===*/
#include "trick_utils/math/include/trick_math_proto.h"

#include "../include/ball_altimeter.h"

int ball_altimeter(
                /* RETURN: -- Always return zero */
BALT * A ,      /* INOUT:  -- Ball altitude sensing parameters */
double * ypos ) /* IN:     M  Ball vertical position from EOM */
{

    BALT_IN * AI = &(A->input) ;
    BALT_OUT * AO = &(A->output) ;

    /* PASS EOM VERTICAL POSITION TO SENSED ALTITUDE POSITION */
    AO->altitude = *ypos ;

    if( AI->add_noise == Yes ) /* ADD 3 SIGMA NOISE */
        AO->altitude += drandom_gaussian( AI->noise , 3 ) ;

    if( AI->add_bias == Yes ) /* ADD BIAS */
        AO->altitude += AI->bias ;

    return( 0 ) ;
}

