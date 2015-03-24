/********************************* TRICK HEADER *******************************
PURPOSE:
    (Print the ball output position)
CLASS:
    (scheduled)
LIBRARY DEPENDENCY:
    ((ball_print.o))
PROGRAMMERS:
    (((Your Name) (Company Name) (Date) (Trick tutorial)))
*******************************************************************************/

     /* GLOBAL DATA STRUCTURE DECLARATIONS */
#include <stdio.h>
#include "ball/L1/include/ball_state.h"
#include "sim_services/Executive/include/exec_proto.h"
#include "sim_services/Message/include/message_proto.h"
#include "ball/L1/include/ball_proto.h"

     /* ENTRY POINT */
int ball_print( /* RETURN: -- Integration multi-step id */
  BSTATE *S )   /* INOUT:  -- Ball EOM state parameters */
{
    if (! S->input.print_off) {

        /* GET SHORTHAND NOTATION FOR DATA STRUCTURES */
        BSTATE_OUT *SO = &(S->output) ;

        message_publish(0, "time = %8.2f , position = %12.6f , %12.6f\n", exec_get_sim_time() , SO->position[0] , SO->position[1]) ;
    }

    return( 0 ) ;
}

