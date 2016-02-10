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
 * Defines member functions for the class RK2MidpointIntegratorConstructor.
 */

/*
Purpose: ()
*/

// System includes

// Integration includes
#include "er7_utils/integration/core/include/integrator_constructor_utils.hh"
#include "er7_utils/integration/core/include/single_cycle_integration_controls.hh"

// Model includes
#include "../include/rk2_midpoint_integrator_constructor.hh"
#include "../include/rk2_midpoint_first_order_ode_integrator.hh"
#include "../include/rk2_midpoint_second_order_ode_integrator.hh"


namespace er7_utils {

// Named constructor; create an RK2MidpointIntegratorConstructor.
IntegratorConstructor*
RK2MidpointIntegratorConstructor::create_constructor (
   void)
{
   return alloc::allocate_object<RK2MidpointIntegratorConstructor> ();
}


// Create a duplicate of the constructor.
IntegratorConstructor *
RK2MidpointIntegratorConstructor::create_copy (
   void)
const
{
   return alloc::replicate_object (*this);
}


// Create an RK2Midpoint integration controls.
IntegrationControls *
RK2MidpointIntegratorConstructor::create_integration_controls (
   void)
const
{
   return integ_utils::allocate_controls<SingleCycleIntegrationControls> (2);
}


// Create an RK2Midpoint state integrator for a first order ODE.
FirstOrderODEIntegrator *
RK2MidpointIntegratorConstructor::create_first_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<RK2MidpointFirstOrderODEIntegrator> (
             size, controls);
}


// Create an RK2Midpoint state integrator for a second order ODE.
SecondOrderODEIntegrator *
RK2MidpointIntegratorConstructor::create_second_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                RK2MidpointSimpleSecondOrderODEIntegrator> (size, controls);
}


// Create an RK2Midpoint state integrator for a second order ODE.
SecondOrderODEIntegrator *
RK2MidpointIntegratorConstructor::
create_generalized_deriv_second_order_ode_integrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionDerivativeFunctions & deriv_funs,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                RK2MidpointGeneralizedDerivSecondOrderODEIntegrator> (
             position_size, velocity_size, deriv_funs, controls);
}


// Create an RK2Midpoint state integrator for a second order ODE.
SecondOrderODEIntegrator *
RK2MidpointIntegratorConstructor::
create_generalized_step_second_order_ode_integrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionStepFunctions & step_funs,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                RK2MidpointGeneralizedStepSecondOrderODEIntegrator> (
             position_size, velocity_size, step_funs, controls);
}

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
