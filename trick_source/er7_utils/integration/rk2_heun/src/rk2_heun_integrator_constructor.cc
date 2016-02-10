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
 * Defines member functions for the class RK2HeunIntegratorConstructor.
 */

/*
Purpose: ()
*/

// System includes

// Integration includes
#include "er7_utils/integration/core/include/integrator_constructor_utils.hh"
#include "er7_utils/integration/core/include/single_cycle_integration_controls.hh"

// Model includes
#include "../include/rk2_heun_integrator_constructor.hh"
#include "../include/rk2_heun_first_order_ode_integrator.hh"
#include "../include/rk2_heun_second_order_ode_integrator.hh"


namespace er7_utils {

// Named constructor; create an RK2HeunIntegratorConstructor.
IntegratorConstructor*
RK2HeunIntegratorConstructor::create_constructor (
   void)
{
   return alloc::allocate_object<RK2HeunIntegratorConstructor> ();
}


// Create a duplicate of the constructor.
IntegratorConstructor *
RK2HeunIntegratorConstructor::create_copy (
   void)
const
{
   return alloc::replicate_object (*this);
}


// Create an RK2Heun integration controls.
IntegrationControls *
RK2HeunIntegratorConstructor::create_integration_controls (
   void)
const
{
   return integ_utils::allocate_controls<SingleCycleIntegrationControls> (2);
}


// Create an RK2Heun state integrator for a first order ODE.
FirstOrderODEIntegrator *
RK2HeunIntegratorConstructor::create_first_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<RK2HeunFirstOrderODEIntegrator> (
             size, controls);
}


// Create an RK2Heun state integrator for a second order ODE.
SecondOrderODEIntegrator *
RK2HeunIntegratorConstructor::create_second_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                RK2HeunSimpleSecondOrderODEIntegrator> (size, controls);
}


// Create an RK2Heun state integrator for a second order ODE.
SecondOrderODEIntegrator *
RK2HeunIntegratorConstructor::
create_generalized_deriv_second_order_ode_integrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionDerivativeFunctions & deriv_funs,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                RK2HeunGeneralizedDerivSecondOrderODEIntegrator> (
             position_size, velocity_size, deriv_funs, controls);
}


// Create an RK2Heun state integrator for a second order ODE.
SecondOrderODEIntegrator *
RK2HeunIntegratorConstructor::
create_generalized_step_second_order_ode_integrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionStepFunctions & step_funs,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                RK2HeunGeneralizedStepSecondOrderODEIntegrator> (
             position_size, velocity_size, step_funs, controls);
}

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
