/********************************* TRICK HEADER *******************************
PURPOSE:
    (This routine prints out final target information at shutdown.)
REFERENCE:
    (((None)))
ASSUMPTIONS AND LIMITATIONS:
    ((Many.))
CLASS:
    (shutdown)
LIBRARY DEPENDENCY:
    ((target_print.o))
PROGRAMMERS:
    (((Edwin Z. Crues) (NASA) (Nov 2009) (--) (Initial version.)))
*******************************************************************************/

/* System includes. */
#include <stdio.h>

/* Trick includes. */

/* Model includes */
#include "../include/target_body.h"

/* ENTRY POINT */
void
target_print(                    /* RETURN: -- None.                       */
   double                time,   /* IN: -- Final time of flight.           */
   TargetBodyInit      * init,   /* IN: -- Target body initializtion data. */
   TargetBodyState     * state,  /* IN: -- Target body state data.         */
   TargetBodyIteration * data  ) /* IN: -- Targetting data.                */
{

   printf( "-----------------------------------------\n" );
   printf( "Initial Position:      x = %f km\n",
           init->position[0]/1000.0 );
   printf( "                       y = %f km\n",
           init->position[1]/1000.0 );
   printf( "                       z = %f km\n\n",
           init->position[2]/1000.0 );
   printf( "Initial Velocity:      x = %f km/s\n",
           init->velocity[0]/1000.0 );
   printf( "                       y = %f km/s\n",
           init->velocity[1]/1000.0 );
   printf( "                       z = %f km/s\n\n",
           init->velocity[2]/1000.0 );
   printf( "Final Position:        x = %f km\n",
           state->position[0]/1000.0 );
   printf( "                       y = %f km\n",
           state->position[1]/1000.0 );
   printf( "                       z = %f km\n\n",
           state->position[2]/1000.0 );
   printf( "Distance from Target:    = %f km\n\n",
           data->delta_r_mag/1000.0 );
   printf( "Delta V:               x = %f km/s\n",
           data->delta_v[0]/1000.0 );
   printf( "                       y = %f km/s\n",
           data->delta_v[1]/1000.0 );
   printf( "                       z = %f km/s\n\n",
           data->delta_v[2]/1000.0 );
   printf( "New Velocity:          x = %.10f km/s\n",
           (init->velocity[0]+data->delta_v[0])/1000.0 );
   printf( "                       y = %.10f km/s\n",
           (init->velocity[1]+data->delta_v[1])/1000.0 );
   printf( "                       z = %.10f km/s\n\n",
           (init->velocity[2]+data->delta_v[2])/1000.0 );
   printf( "Time of Flight:          = %f s\n", time );
   printf( "-----------------------------------------\n");

   /* Return to calling function. */
   return;
}

