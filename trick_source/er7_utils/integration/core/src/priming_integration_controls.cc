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
 * Defines the methods for class PrimingIntegrationControls.
 */

/*
Purpose: ()
*/

// System includes
#include <cstddef>

// Interface includes
#include "er7_utils/interface/include/alloc.hh"

// Local includes
#include "../include/integrator_constructor.hh"
#include "../include/priming_integration_controls.hh"


namespace er7_utils {


// PrimingIntegrationControls non-default constructor.
PrimingIntegrationControls::PrimingIntegrationControls (
   const IntegratorConstructor & primer_constructor,
   unsigned int history_buffer_size,
   unsigned int number_operating_stages)
:
   StandardIntegrationControls (number_operating_stages),
   priming_controls (primer_constructor.create_integration_controls()),
   priming_count (history_buffer_size),
   history_length (history_buffer_size)
{
   set_alt_controls (*priming_controls);
}


// Destructor.
PrimingIntegrationControls::~PrimingIntegrationControls (
   void)
{
   alloc::delete_object (priming_controls);
}


// Non-throwing swap.
void
PrimingIntegrationControls::swap (
   PrimingIntegrationControls & other)
{
   StandardIntegrationControls::swap (other);
   std::swap (priming_controls, other.priming_controls);
   std::swap (priming_count, other.priming_count);
   std::swap (history_length, other.history_length);
}


// Clone a PrimingIntegrationControls.
PrimingIntegrationControls *
PrimingIntegrationControls::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Reset the integration controls.
void
PrimingIntegrationControls::reset_integrator (
   void)
{
   priming_controls->reset_integrator();
   priming_count = history_length;
   set_alt_controls (*priming_controls);
}


// Start an integration tour.
void
PrimingIntegrationControls::start_integration_tour (
   void)
{
   // Cycle and tour are synonomous for this class of integration controls.
   // Set the cycle start and delta times to their tour counterparts.
   StandardIntegrationControls::start_integration_tour();

   // Not yet primed: Bump the priming count and check if we are now primed.
   if (priming_count > 0) {
      --priming_count;
      // Newly primed: Adjust the transition table so the transition from
      // stage zero is to the initial operational stage.
      if (priming_count == 0) {
         clear_alt_controls ();
      }
   }
}

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
