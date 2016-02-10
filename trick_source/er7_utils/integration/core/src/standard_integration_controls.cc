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

// System includes
#include <algorithm>

// Interface includes
#include "er7_utils/interface/include/alloc.hh"

// Local includes
#include "../include/base_integration_group.hh"
#include "../include/integrator_interface.hh"
#include "../include/time_interface.hh"
#include "../include/standard_integration_controls.hh"


namespace er7_utils {


// Non-throwing swap.
void
StandardIntegrationControls::swap (
   StandardIntegrationControls & other)
{
   IntegrationControls::swap (other);
   std::swap (cycle_starttime, other.cycle_starttime);
   std::swap (cycle_simdt, other.cycle_simdt);
   std::swap (cycle_dyndt, other.cycle_dyndt);
   std::swap (multi_cycle, other.multi_cycle);
   /* std::swap (target_attained, other.target_attained); */
   std::swap (alt_controls, other.alt_controls);
}


// Clone a StandardIntegrationControls.
StandardIntegrationControls *
StandardIntegrationControls::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Perform one step of the integration process.
unsigned int
StandardIntegrationControls::integrate (
   double starttime,
   double sim_dt,
   TimeInterface & time_interface,
   IntegratorInterface & integ_interface,
   BaseIntegrationGroup & integ_group)
{

   // Starting a new integration tour needs special processing.
   if (step_number == 0) {

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

      // Cache the integration tour start time.
      integ_starttime = starttime;

      // Perform start of tour operations for a multi-cycle integrator.
      if (multi_cycle) {
         start_integration_tour ();
      }

      // Standard single-cycle integrator: Set the integration cycle times.
      else {
         cycle_starttime  = integ_starttime;
         cycle_simdt      = integ_simdt;
         cycle_dyndt      = integ_dyndt;
         integ_simtime    = integ_starttime;
         integ_time_scale = 0.0;
      }
   }


   // No alternative integrator:
   // Control the main body of the integration step.
   if (alt_controls == NULL) {

      // Set the target_stage based on the transition table and cycle_stage.
       unsigned int target_stage = transition_table[cycle_stage];

      // Integrate the bodies that comprise the group to the target stage.
      // The group integrator returns true if all integrators succeeded.
      const IntegratorResult & integ_status =
         integ_group.integrate_bodies (integ_dyndt, target_stage);
      bool target_attained = integ_status.get_passed();
      double time_scale = integ_status.get_time_scale();

      // Advance time to the next stage.
      if (integ_time_scale != time_scale) {
         integ_time_scale = time_scale;
         integ_simtime    = cycle_starttime + time_scale*cycle_simdt;
         time_interface.update_time (integ_simtime);
      }

      // Target attained by all integrators:
      // Advance the cycle stage to the attained target stage.
      if (target_attained) {
         cycle_stage = target_stage;
      }

      // FUTURE FIXME:
      // Perform a technique-specific action if the target was not attained.
      // For now, the else is intentionally empty.
      else {
      }

      // At the end of a cycle:
      // Reset cycle_stage, and advance or reset the step_number.
      if (cycle_stage == final_stage) {

         cycle_stage = 0;

         // Reset the step number to zero for a single-cycle integrator
         // or for a multi-cycle integrator that is at the end of its tour.
         if ((! multi_cycle) || end_integration_cycle()) {
            step_number = 0;
         }

         // Multi-cycle integrator that isn't finished: Bump the step number.
         else {
            step_number++;
         }
      }

      // Not at the end of the cycle: Bump the step number.
      else {
         step_number++;
      }

      // Update the sim engine's integration interface structure.
      integ_group.update_integration_interface (step_number);
   }

   // Non-standard: Use the alternate integrator.
   else {
      step_number = alt_controls->integrate (
                       starttime, sim_dt,
                       time_interface, integ_interface, integ_group);
   }

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
