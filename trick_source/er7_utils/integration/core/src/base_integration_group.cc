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
 * Defines BaseIntegrationGroup methods.
 */

/*
Purpose: ()
*/


// System includes
#include <algorithm>
#include <cstddef>

// Interface includes
#include "er7_utils/interface/include/alloc.hh"
#include "er7_utils/interface/include/message_handler.hh"

// Local includes
#include "../include/base_integration_group.hh"
#include "../include/bogus_integration_controls.hh"
#include "../include/integration_controls.hh"
#include "../include/integration_messages.hh"
#include "../include/integrator_constructor.hh"
#include "../include/integrator_interface.hh"
#include "../include/time_interface.hh"


namespace er7_utils {

// BaseIntegrationGroup default constructor.
// The integ_controls member is initialized to a BogusIntegrationControls
// instance so that attempts to use an uninitialized group will fail
// rather than crash.
BaseIntegrationGroup::BaseIntegrationGroup (
   void)
:
   integ_constructor (NULL),
   integ_interface (NULL),
   time_interface (NULL),
   integ_controls (BogusIntegrationControls().create_copy())
{
}


// BaseIntegrationGroup non-default constructor.
BaseIntegrationGroup::BaseIntegrationGroup (
   IntegratorConstructor & integ_cotr,
   IntegratorInterface & integ_inter,
   TimeInterface & time_if)
:
   integ_constructor (&integ_cotr),
   integ_interface (&integ_inter),
   time_interface (&time_if),
   integ_controls (BogusIntegrationControls().create_copy())
{
}


// BaseIntegrationGroup copy constructor.
// This is probably bogus.
BaseIntegrationGroup::BaseIntegrationGroup (
   const BaseIntegrationGroup & source)
:
   integ_constructor (source.integ_constructor),
   integ_interface (source.integ_interface),
   time_interface (source.time_interface),
   integ_controls (source.integ_controls ?
                   source.integ_controls->create_copy() :
                   BogusIntegrationControls().create_copy())
{
}


// Non-throwing swap.
void
BaseIntegrationGroup::swap (
   BaseIntegrationGroup & other)
{
   std::swap (other.integ_constructor, integ_constructor);
   std::swap (other.integ_interface, integ_interface);
   std::swap (other.time_interface, time_interface);
   std::swap (other.integ_controls, integ_controls);
}


// BaseIntegrationGroup destructor.
BaseIntegrationGroup::~BaseIntegrationGroup (
   void)
{
   Er7UtilsDeletable::delete_instance (integ_controls);
}


// Initialize the integration group.
void
BaseIntegrationGroup::initialize_group (
   void)
{
   // Protect against a null integ_constructor or integ_interface.
   // Derived classes should ensure that this can't happen.
   if ((integ_constructor == NULL) || (integ_interface == NULL)) {
      MessageHandler::error (
         __FILE__, __LINE__,
         IntegrationMessages::invalid_request,
         "Attempt to initialize an improperly constructed integration group.\n"
         "No initialization was performed.\n");
      return;
   }

   // Destroy the existing (if any) integration controls.
   Er7UtilsDeletable::delete_instance (integ_controls);

   // Create an integration controls via the integrator constructor.
   integ_controls = integ_constructor->create_integration_controls();

   // Tell the simulation engine whether to calculate derivatives
   // on the initial step of an integration cycle.
   integ_interface->set_first_step_derivs_flag (
      integ_constructor->need_first_step_derivatives());

   // But we always need derivatives on the very first step.
   integ_interface->reset_first_step_derivs_flag ();
}


}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
