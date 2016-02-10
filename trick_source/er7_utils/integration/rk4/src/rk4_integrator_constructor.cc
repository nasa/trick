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
 * Defines member functions for the class RK4IntegratorConstructor.
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
#include "../include/rk4_integrator_constructor.hh"
#include "../include/rk4_first_order_ode_integrator.hh"
#include "../include/rk4_second_order_ode_integrator.hh"


namespace er7_utils {

// Named constructor; create an RK4IntegratorConstructor.
IntegratorConstructor*
RK4IntegratorConstructor::create_constructor (
   void)
{
   return alloc::allocate_object<RK4IntegratorConstructor> ();
}


// Create a duplicate of the constructor.
IntegratorConstructor *
RK4IntegratorConstructor::create_copy (
   void)
const
{
   return alloc::replicate_object (*this);
}


// Create an RK4 integration controls.
IntegrationControls *
RK4IntegratorConstructor::create_integration_controls (
   void)
const
{
   return integ_utils::allocate_controls<SingleCycleIntegrationControls> (4);
}


// Create an RK4 state integrator for a first order ODE.
FirstOrderODEIntegrator *
RK4IntegratorConstructor::create_first_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<RK4FirstOrderODEIntegrator> (
             size, controls);
}


// Create an RK4 state integrator for a second order ODE.
SecondOrderODEIntegrator *
RK4IntegratorConstructor::create_second_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<RK4SimpleSecondOrderODEIntegrator> (
             size, controls);
}


// Create an RK4 state integrator for a second order ODE.
SecondOrderODEIntegrator *
RK4IntegratorConstructor::
create_generalized_deriv_second_order_ode_integrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionDerivativeFunctions & deriv_funs,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                RK4GeneralizedDerivSecondOrderODEIntegrator> (
             position_size, velocity_size, deriv_funs, controls);
}


// Create an RK4 state integrator for a second order ODE.
SecondOrderODEIntegrator *
RK4IntegratorConstructor::
create_generalized_step_second_order_ode_integrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionStepFunctions & step_funs,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                RK4GeneralizedStepSecondOrderODEIntegrator> (
             position_size, velocity_size, step_funs, controls);
}


}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
