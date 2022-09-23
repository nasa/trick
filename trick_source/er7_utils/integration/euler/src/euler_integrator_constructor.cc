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
 * Defines member functions for the class EulerIntegratorConstructor.
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
#include "../include/euler_integrator_constructor.hh"
#include "../include/euler_first_order_ode_integrator.hh"
#include "../include/euler_second_order_ode_integrator.hh"


namespace er7_utils {

// Named constructor; create an EulerIntegratorConstructor.
IntegratorConstructor*
EulerIntegratorConstructor::create_constructor (
   void)
{
   return alloc::allocate_object<EulerIntegratorConstructor> ();
}


// Create a duplicate of the constructor.
IntegratorConstructor *
EulerIntegratorConstructor::create_copy (
   void)
const
{
   return alloc::replicate_object (*this);
}


// Create an integration controls for Euler.
IntegrationControls *
EulerIntegratorConstructor::create_integration_controls (
   void)
const
{
   return integ_utils::allocate_controls<SingleCycleIntegrationControls> (1);
}


// Create an Euler integrator for a first order ODE.
FirstOrderODEIntegrator *
EulerIntegratorConstructor::create_first_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<EulerFirstOrderODEIntegrator> (
             size, controls);
}


// Create an Euler integrator for a second order ODE.
SecondOrderODEIntegrator *
EulerIntegratorConstructor::create_second_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                EulerSimpleSecondOrderODEIntegrator> (
             size, controls);
}


// Create an Euler integrator for a second order ODE.
SecondOrderODEIntegrator *
EulerIntegratorConstructor::
create_generalized_deriv_second_order_ode_integrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionDerivativeFunctions & deriv_funs,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                EulerGeneralizedDerivSecondOrderODEIntegrator> (
             position_size, velocity_size, deriv_funs, controls);
}


// Create an Euler integrator for a second order ODE.
SecondOrderODEIntegrator *
EulerIntegratorConstructor::
create_generalized_step_second_order_ode_integrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionStepFunctions & step_funs,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                EulerGeneralizedStepSecondOrderODEIntegrator> (
             position_size, velocity_size, step_funs, controls);
}

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
