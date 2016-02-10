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
#include <cstddef>

// Interface includes
#include "er7_utils/interface/include/alloc.hh"

// Local includes
#include "../include/integration_controls.hh"


namespace er7_utils {


// IntegrationControls default constructor.
IntegrationControls::IntegrationControls (
   void)
:
   Er7UtilsDeletable(),
   time_scale_factor(0.0),
   integ_starttime(0.0),
   integ_simdt(0.0),
   integ_dyndt(0.0),
   integ_time_scale(0.0),
   integ_simtime(0.0),
   transition_table(NULL),
   step_number(0),
   cycle_stage(0),
   /* target_stage(0), */
   final_stage(0),
   number_stages(0),
   reset_needed(true)
{ }


// IntegrationControls copy constructor.
IntegrationControls::IntegrationControls (
   const IntegrationControls & source)
:
   Er7UtilsDeletable(),
   time_scale_factor (source.time_scale_factor),
   integ_starttime (source.integ_starttime),
   integ_simdt (source.integ_simdt),
   integ_dyndt (source.integ_dyndt),
   integ_time_scale (source.integ_time_scale),
   integ_simtime (source.integ_simtime),
   transition_table (NULL),
   step_number (source.step_number),
   cycle_stage (source.cycle_stage),
   /* target_stage (source.target_stage), */
   final_stage (source.final_stage),
   number_stages (source.number_stages),
   reset_needed (source.reset_needed)
{
   // Allocate and copy the source's transition table if it has one.
   if (source.transition_table != NULL) {
      transition_table = alloc::allocate_array<unsigned int> (number_stages);
      for (unsigned int ii = 0; ii < number_stages; ++ii) {
         transition_table[ii] = source.transition_table[ii];
      }
   }
}


// IntegrationControls non-default constructor.
IntegrationControls::IntegrationControls (
   unsigned int number_stages_in)
:
   Er7UtilsDeletable(),
   time_scale_factor(0.0),
   integ_starttime(0.0),
   integ_simdt(0.0),
   integ_dyndt(0.0),
   integ_time_scale(0.0),
   integ_simtime(0.0),
   transition_table(NULL),
   step_number(0),
   cycle_stage(0),
   /* target_stage(0), */
   final_stage(number_stages_in),
   number_stages(number_stages_in),
   reset_needed(true)
{
   // Create the default transition table, state n -> n+1
   transition_table = alloc::allocate_array<unsigned int> (number_stages);
   for (unsigned int ii = 0; ii < number_stages; ++ii) {
      transition_table[ii] = ii+1;
   }
}


// Non-throwing swap.
void
IntegrationControls::swap (
   IntegrationControls & other)
{
   std::swap (other.time_scale_factor, time_scale_factor);
   std::swap (other.integ_starttime, integ_starttime);
   std::swap (other.integ_simdt, integ_simdt);
   std::swap (other.integ_dyndt, integ_dyndt);
   std::swap (other.integ_time_scale, integ_time_scale);
   std::swap (other.integ_simtime, integ_simtime);
   std::swap (other.transition_table, transition_table);
   std::swap (other.step_number, step_number);
   std::swap (other.cycle_stage, cycle_stage);
   /* std::swap (other.target_stage, target_stage); */
   std::swap (other.final_stage, final_stage);
   std::swap (other.number_stages, number_stages);
   std::swap (other.reset_needed, reset_needed);
}


// IntegrationControls destructor.
IntegrationControls::~IntegrationControls (
   void)
{
   alloc::deallocate_array (transition_table);
}


}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
