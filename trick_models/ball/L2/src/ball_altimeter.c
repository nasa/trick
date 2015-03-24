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

