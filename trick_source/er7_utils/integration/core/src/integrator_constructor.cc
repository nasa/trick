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
 * Defines member functions for the IntegratorConstructor class.
 */

/*
Purpose: ()
*/

// Local includes
#include "../include/integrator_constructor.hh"

#include "../include/integration_messages.hh"
#include "../include/integrator_result.hh"
#include "../include/integrator_result_merger.hh"

// Interface includes
#include "er7_utils/interface/include/alloc.hh"
#include "er7_utils/interface/include/message_handler.hh"

// System includes
#include <cmath>


namespace er7_utils {


// Create an integrator results merger object.
IntegratorResultMerger *
IntegratorConstructor::create_integrator_results_merger (
   void)
const
{
   return alloc::allocate_object<IntegratorResultMerger> ();
}


// Create an integrator for a first order ODE.
FirstOrderODEIntegrator *
IntegratorConstructor::create_first_order_ode_integrator (
   unsigned int,
   IntegrationControls &)
const
{
   MessageHandler::error (
      __FILE__, __LINE__,
      IntegrationMessages::unsupported_option,
      "Unable to create a first order ODE integrator for %s.",
      get_class_name());

   return NULL;
}


// Create an integrator for a second order ODE.
SecondOrderODEIntegrator *
IntegratorConstructor::create_second_order_ode_integrator (
   unsigned int,
   IntegrationControls &)
const
{
   MessageHandler::error (
      __FILE__, __LINE__,
      IntegrationMessages::unsupported_option,
      "Unable to create a simple second order ODE integrator for %s.",
      get_class_name());

   return NULL;
}


// Create an integrator for a second order ODE.
SecondOrderODEIntegrator *
IntegratorConstructor::create_generalized_deriv_second_order_ode_integrator (
   unsigned int,
   unsigned int,
   const GeneralizedPositionDerivativeFunctions &,
   IntegrationControls &)
const
{
   MessageHandler::error (
      __FILE__, __LINE__,
      IntegrationMessages::unsupported_option,
      "Unable to create a generalized second order ODE integrator for %s.",
      get_class_name());

   return NULL;
}


// Create an integrator for a second order ODE.
SecondOrderODEIntegrator *
IntegratorConstructor::create_generalized_step_second_order_ode_integrator (
   unsigned int,
   unsigned int,
   const GeneralizedPositionStepFunctions &,
   IntegrationControls &)
const
{
   MessageHandler::error (
      __FILE__, __LINE__,
      IntegrationMessages::unsupported_option,
      "Unable to create a generalized second order ODE integrator for %s.",
      get_class_name());

   return NULL;
}


}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
