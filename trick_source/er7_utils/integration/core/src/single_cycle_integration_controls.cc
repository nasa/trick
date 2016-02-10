/**
 * @if Er7UtilsUseGroups
 * @addtogroup Er7Utils
 * @{
 * @addtogroup Integration
 * @{
 * @endif
 */

/**
 * @file
 * Defines the methods for class IntegrationControls.
 */

/*
Purpose: ()
*/


// Interface includes
#include "er7_utils/interface/include/alloc.hh"

// Local includes
#include "../include/base_integration_group.hh"
#include "../include/integrator_interface.hh"
#include "../include/time_interface.hh"
#include "../include/single_cycle_integration_controls.hh"


namespace er7_utils {


// Clone a SingleCycleIntegrationControls.
SingleCycleIntegrationControls *
SingleCycleIntegrationControls::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Perform one step of the integration process.
unsigned int
SingleCycleIntegrationControls::integrate (
   double starttime,
   double sim_dt,
   TimeInterface & time_interface,
   IntegratorInterface & integ_interface,
   BaseIntegrationGroup & integ_group)
{

   // Starting a new integration tour/cycle needs special processing.
   if (cycle_stage == 0) {

      // Reset the integrators, time if the meaning of time has changed.
      if (reset_needed || (integ_simdt != sim_dt)) {

         // Reset integrators.
         integ_group.reset_body_integrators ();
         reset_integrator ();

         // Update timing information.
         time_scale_factor = time_interface.get_time_scale_factor();
         integ_simdt = sim_dt;
         integ_dyndt = sim_dt * time_scale_factor;

         // Mark the reset as handled.
         reset_needed = false;
         integ_interface.restore_first_step_derivs_flag ();
      }

      // Set time to the start of the interval.
      integ_simtime = integ_starttime = starttime;
      integ_time_scale = 0.0;
   }

   // Advance the stage per the transition table.
   unsigned int target_stage = transition_table[cycle_stage];

   // Integrate the dynamic bodies to the target stage and advance time.
   // This simple controller assumes integration always succeeds.
   const IntegratorResult & integ_status =
      integ_group.integrate_bodies (integ_dyndt, target_stage);
   double time_scale = integ_status.get_time_scale();

   // Advance time.
   if (integ_time_scale != time_scale) {
      integ_time_scale = time_scale;
      integ_simtime    = integ_starttime + time_scale*integ_simdt;
      time_interface.update_time (integ_simtime);
   }

   // Reset cycle_stage, step_number at the end of a cycle.
   if (target_stage == final_stage) {
      cycle_stage = 0;
      step_number = 0;
      time_interface.update_time(integ_starttime+integ_simdt);
   }
   else {
      cycle_stage = target_stage;
      step_number++;
   }

   // Update the sim engine's integration interface structure.
   integ_group.update_integration_interface (step_number);

   // Return the step number, which is zero upon completion.
   return step_number;
}

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
