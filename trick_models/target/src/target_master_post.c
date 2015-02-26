/********************************* TRICK HEADER *******************************
PURPOSE:
    (This routine runs at the end of each targeting iteration loop on the
     Master side of the simulation.)
REFERENCE:
    (((None)))
ASSUMPTIONS AND LIMITATIONS:
    ((Many.))
CLASS:
    (monte_master_post)
LIBRARY DEPENDENCY:
    ((target_master_post.o))
PROGRAMMERS:
    (((Edwin Z. Crues) (NASA) (Nov 2009) (--) (Initial version.)))
*******************************************************************************/

/* System includes. */
#include <stdio.h>

/* Trick includes. */
#include "sim_services/MonteCarlo/include/montecarlo_c_intf.h"
#include "sim_services/UnitTest/include/trick_tests.h"
#include "trick_utils/comm/include/tc_proto.h"
#include "trick_utils/math/include/vector_macros.h"

/* Model includes */
#include "../include/target_body.h"

/* ENTRY POINT */
int target_master_post(               
   TCDevice* tc_dev,   
   TargetBodyInit* init,     
   TargetBodyState* state,   
   TargetBodyData* data,    
   TargetBodyIteration* iterate )
{
   char message[128];
   TargetBodyIteration iter_ret;

   printf("\e[33mtarget_master_post\(\)\e[00m\n");
   /* Read slave's results */ 
   tc_read( mc_get_connection_device(), (char*)&iter_ret, sizeof(TargetBodyIteration) );

   /* Copy iteration data. */
   iterate->delta_r_mag = iter_ret.delta_r_mag;
   V_COPY( iterate->delta_r, iter_ret.delta_r );
   V_COPY( iterate->delta_v, iter_ret.delta_v );
   V_COPY( iterate->v_init, iter_ret.v_init );

   /* Increment the iteration count. */
   iterate->iter_cnt++;

   /* Check for convergence. */
   if ( iterate->delta_r_mag < iterate->tolerance ) {

      /* The below if-else block is for Hudson testing to verify 
         the correct number of runs occurs before termination */
      if (mc_get_current_run() == 5) {
          add_test_result("SIM_target Simulation", "MonteCarlo Optimization", "");
      } else {
          add_test_result("SIM_target Simulation", "MonteCarlo Optimization", "Should be 5 runs");
      }

      trick_test_add_parent("SIM_target Simulation", "MonteCarlo Optimization", "2014948908");

      call_write_output() ;

      sprintf( message,
               "Iteration loop (%d) converged with Delta r_mag = %g\n",
               iterate->iter_cnt, iterate->delta_r_mag );
      exec_terminate( __FILE__, message );
   }

   /* Check to see if the maximum iteration count is exceeded. */
   if ( iterate->iter_cnt >= iterate->iter_max ) {
      sprintf( message,
               "Iteration loop count limit (%d) exceeded.\n",
               iterate->iter_cnt );
      exec_terminate( __FILE__, message );
   }

   /* Let's print out some debug information. */
   printf( "-----------------------------------------\n" );
   printf( "Initial Position:      x = %f km\n",
           state->position[0]/1000.0 );
   printf( "                       y = %f km\n",
           state->position[1]/1000.0 );
   printf( "                       z = %f km\n\n",
           state->position[2]/1000.0 );
   printf( "Initial Velocity:      x = %f km/s\n",
           state->velocity[0]/1000.0 );
   printf( "                       y = %f km/s\n",
           state->velocity[1]/1000.0 );
   printf( "                       z = %f km/s\n\n",
           state->velocity[2]/1000.0 );
   printf( "Distance from Target:    = %f km\n\n",
           iterate->delta_r_mag/1000.0 );
   printf( "Delta r:               x = %f km\n",
           iterate->delta_r[0]/1000.0 );
   printf( "                       y = %f km\n",
           iterate->delta_r[1]/1000.0 );
   printf( "                       z = %f km\n\n",
           iterate->delta_r[2]/1000.0 );
   printf( "Delta V:               x = %f km/s\n",
           iterate->delta_v[0]/1000.0 );
   printf( "                       y = %f km/s\n",
           iterate->delta_v[1]/1000.0 );
   printf( "                       z = %f km/s\n\n",
           iterate->delta_v[2]/1000.0 );

   /* Since we're here, we must need another iteration. */
   /* So, we compute the new initial velocity.          */
   V_ADD( state->velocity, iterate->v_init, iterate->delta_v );

   printf( "New Velocity:          x = %.10f km/s\n",
           state->velocity[0]/1000.0 );
   printf( "                       y = %.10f km/s\n",
           state->velocity[1]/1000.0 );
   printf( "                       z = %.10f km/s\n\n",
           state->velocity[2]/1000.0 );
   printf( "-----------------------------------------\n");

   /* Return to calling function. */
   return( 0 );

}

