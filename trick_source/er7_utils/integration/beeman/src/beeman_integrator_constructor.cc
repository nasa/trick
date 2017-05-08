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
 * Defines member functions for the class BeemanIntegratorConstructor.
 */

/*
Purpose: ()
*/

// System includes

// Interface includes
#include "er7_utils/interface/include/alloc.hh"

// Integration includes
#include "er7_utils/integration/core/include/integrator_constructor_utils.hh"
#include "er7_utils/integration/core/include/priming_integration_controls.hh"
#include "er7_utils/integration/rk2_heun/include/rk2_heun_first_order_ode_integrator.hh"

// Model includes
#include "../include/beeman_integrator_constructor.hh"
#include "../include/beeman_second_order_ode_integrator.hh"


namespace er7_utils {

// Named constructor; create an BeemanIntegratorConstructor.
IntegratorConstructor*
BeemanIntegratorConstructor::create_constructor (
   void)
{
   return alloc::allocate_object<BeemanIntegratorConstructor> ();
}


// Create a duplicate of the constructor.
IntegratorConstructor *
BeemanIntegratorConstructor::create_copy (
   void)
const
{
   return alloc::replicate_object (*this);
}


// Create an Beeman integration controls.
IntegrationControls *
BeemanIntegratorConstructor::create_integration_controls (
   void)
const
{
   return integ_utils::allocate_controls<PrimingIntegrationControls> (
             *primer_constructor, 2, 2);
}


// Create a Heun's method state integrator for a first order ODE.
FirstOrderODEIntegrator *
BeemanIntegratorConstructor::create_first_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<RK2HeunFirstOrderODEIntegrator> (
             size, controls);
}


// Create an Beeman state integrator for a second order ODE.
SecondOrderODEIntegrator *
BeemanIntegratorConstructor::create_second_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                BeemanSimpleSecondOrderODEIntegrator> (
             *primer_constructor, size, controls);
}


// Create an Beeman state integrator for a second order ODE.
SecondOrderODEIntegrator *
BeemanIntegratorConstructor::
create_generalized_deriv_second_order_ode_integrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionDerivativeFunctions & deriv_funs,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                BeemanGeneralizedDerivSecondOrderODEIntegrator> (
             *primer_constructor,
             position_size, velocity_size, deriv_funs, controls);
}


#if 0
// Create an Beeman state integrator for a second order ODE.
SecondOrderODEIntegrator *
BeemanIntegratorConstructor::
create_generalized_step_second_order_ode_integrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionStepFunctions & step_funs,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                BeemanGeneralizedStepSecondOrderODEIntegrator> (
             *primer_constructor,
             position_size, velocity_size, step_funs, controls);
}
#endif


}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
