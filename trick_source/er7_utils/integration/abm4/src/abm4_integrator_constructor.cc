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
 * Defines member functions for the class ABM4IntegratorConstructor.
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
#include "er7_utils/integration/rk4/include/rk4_integrator_constructor.hh"

// Model includes
#include "../include/abm4_integrator_constructor.hh"
#include "../include/abm4_first_order_ode_integrator.hh"
#include "../include/abm4_second_order_ode_integrator.hh"


namespace er7_utils {

// Named constructor; create an ABM4IntegratorConstructor.
IntegratorConstructor*
ABM4IntegratorConstructor::create_constructor (
   void)
{
   return alloc::allocate_object<ABM4IntegratorConstructor> ();
}


// Create a duplicate of the constructor.
IntegratorConstructor *
ABM4IntegratorConstructor::create_copy (
   void)
const
{
   return alloc::replicate_object (*this);
}


// Create an ABM4 integration controls.
IntegrationControls *
ABM4IntegratorConstructor::create_integration_controls (
   void)
const
{
   return integ_utils::allocate_controls<PrimingIntegrationControls> (
             *primer_constructor, 4, 2);
}


// Create an ABM4 state integrator for a first order ODE.
FirstOrderODEIntegrator *
ABM4IntegratorConstructor::create_first_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<ABM4FirstOrderODEIntegrator> (
             *primer_constructor, size, controls);
}


// Create an ABM4 state integrator for a second order ODE.
SecondOrderODEIntegrator *
ABM4IntegratorConstructor::create_second_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<ABM4SimpleSecondOrderODEIntegrator> (
             *primer_constructor, size, controls);
}


// Create an ABM4 state integrator for a second order ODE.
SecondOrderODEIntegrator *
ABM4IntegratorConstructor::
create_generalized_deriv_second_order_ode_integrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionDerivativeFunctions & deriv_funs,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                ABM4GeneralizedDerivSecondOrderODEIntegrator> (
             *primer_constructor,
             position_size, velocity_size, deriv_funs, controls);
}


#if 0
// Create an ABM4 state integrator for a second order ODE.
SecondOrderODEIntegrator *
ABM4IntegratorConstructor::
create_generalized_step_second_order_ode_integrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionStepFunctions & step_funs,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                ABM4GeneralizedStepSecondOrderODEIntegrator> (
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
