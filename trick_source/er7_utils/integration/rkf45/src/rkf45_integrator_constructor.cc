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
 * Defines member functions for the class RKFehlberg45IntegratorConstructor.
 */

/*
Purpose: ()
*/

// System includes

// Interface includes
#include "er7_utils/interface/include/alloc.hh"

// Integration includes
#include "er7_utils/integration/core/include/integrator_constructor_utils.hh"
#include "er7_utils/integration/core/include/single_cycle_integration_controls.hh"

// Model includes
#include "../include/rkf45_integrator_constructor.hh"
#include "../include/rkf45_first_order_ode_integrator.hh"
#include "../include/rkf45_second_order_ode_integrator.hh"


namespace er7_utils {

// Named constructor; create an RKFehlberg45IntegratorConstructor.
IntegratorConstructor*
RKFehlberg45IntegratorConstructor::create_constructor (
   void)
{
   return alloc::allocate_object<RKFehlberg45IntegratorConstructor> ();
}


// Create a duplicate of the constructor.
IntegratorConstructor *
RKFehlberg45IntegratorConstructor::create_copy (
   void)
const
{
   return alloc::replicate_object (*this);
}


// Create an RKFehlberg45 integration controls.
IntegrationControls *
RKFehlberg45IntegratorConstructor::create_integration_controls (
   void)
const
{
   SingleCycleIntegrationControls * controller =
      integ_utils::allocate_controls<SingleCycleIntegrationControls> (6);
   return controller;
}


// Create an RKFehlberg45 state integrator for a first order ODE.
FirstOrderODEIntegrator *
RKFehlberg45IntegratorConstructor::create_first_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<RKFehlberg45FirstOrderODEIntegrator> (
             size, controls);
}


// Create an RKFehlberg45 state integrator for a second order ODE.
SecondOrderODEIntegrator *
RKFehlberg45IntegratorConstructor::create_second_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<RKFehlberg45SimpleSecondOrderODEIntegrator> (
             size, controls);
}


// Create an RKFehlberg45 state integrator for a second order ODE.
SecondOrderODEIntegrator *
RKFehlberg45IntegratorConstructor::
create_generalized_deriv_second_order_ode_integrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionDerivativeFunctions & deriv_funs,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                RKFehlberg45GeneralizedDerivSecondOrderODEIntegrator> (
             position_size, velocity_size, deriv_funs, controls);
}


// Create an RKFehlberg45 state integrator for a second order ODE.
SecondOrderODEIntegrator *
RKFehlberg45IntegratorConstructor::
create_generalized_step_second_order_ode_integrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionStepFunctions & step_funs,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                RKFehlberg45GeneralizedStepSecondOrderODEIntegrator> (
             position_size, velocity_size, step_funs, controls);
}


}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
