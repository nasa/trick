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
 * Defines member functions for the class VelocityVerletIntegratorConstructor.
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
#include "er7_utils/integration/rk2_heun/include/rk2_heun_first_order_ode_integrator.hh"

// Model includes
#include "../include/velocity_verlet_integrator_constructor.hh"
#include "../include/velocity_verlet_second_order_ode_integrator.hh"


namespace er7_utils {

// Named constructor; create an VelocityVerletIntegratorConstructor.
IntegratorConstructor*
VelocityVerletIntegratorConstructor::create_constructor (
   void)
{
   return alloc::allocate_object<VelocityVerletIntegratorConstructor> ();
}


// Create a duplicate of the constructor.
IntegratorConstructor*
VelocityVerletIntegratorConstructor::create_copy (
   void)
const
{
   return alloc::replicate_object (*this);
}


// Create an VelocityVerlet integration controls.
IntegrationControls *
VelocityVerletIntegratorConstructor::create_integration_controls (
   void)
const
{
   return integ_utils::allocate_controls<SingleCycleIntegrationControls> (2);
}


// Create a Heun's method state integrator as a surrogate for velocity verlet
// for a first order ODE.
FirstOrderODEIntegrator *
VelocityVerletIntegratorConstructor::create_first_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<RK2HeunFirstOrderODEIntegrator> (
             size, controls);
}


// Create a velocity verlet state integrator for a second order ODE.
SecondOrderODEIntegrator *
VelocityVerletIntegratorConstructor::create_second_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                VelocityVerletSimpleSecondOrderODEIntegrator> (
             size, controls);
}


// Create a velocity verlet state integrator for a second order ODE.
SecondOrderODEIntegrator *
VelocityVerletIntegratorConstructor::
create_generalized_deriv_second_order_ode_integrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionDerivativeFunctions & deriv_funs,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                VelocityVerletGeneralizedDerivSecondOrderODEIntegrator> (
             position_size, velocity_size, deriv_funs, controls);
}


// Create a velocity verlet state integrator for a second order ODE.
SecondOrderODEIntegrator *
VelocityVerletIntegratorConstructor::
create_generalized_step_second_order_ode_integrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionStepFunctions & step_funs,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                VelocityVerletGeneralizedStepSecondOrderODEIntegrator> (
             position_size, velocity_size, step_funs, controls);
}

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
