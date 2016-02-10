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
 * Defines member functions for the class NystromLear2IntegratorConstructor.
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
#include "er7_utils/integration/rk2_midpoint/include/rk2_midpoint_first_order_ode_integrator.hh"

// Model includes
#include "../include/nl2_integrator_constructor.hh"
#include "../include/nl2_second_order_ode_integrator.hh"


namespace er7_utils {

// Named constructor; create an NystromLear2IntegratorConstructor.
IntegratorConstructor*
NystromLear2IntegratorConstructor::create_constructor (
   void)
{
   return alloc::allocate_object<NystromLear2IntegratorConstructor> ();
}


// Create a duplicate of the constructor.
IntegratorConstructor *
NystromLear2IntegratorConstructor::create_copy (
   void)
const
{
   return alloc::allocate_object<NystromLear2IntegratorConstructor> (*this);
}


// Create an integration controls for Nystrom-Lear 2.
IntegrationControls *
NystromLear2IntegratorConstructor::create_integration_controls (
   void)
const
{
   return integ_utils::allocate_controls<SingleCycleIntegrationControls> (2);
}


// Create an RK2 midpoint integrator for a first order ODE.
FirstOrderODEIntegrator *
NystromLear2IntegratorConstructor::create_first_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<RK2MidpointFirstOrderODEIntegrator> (
             size, controls);
}


// Create a Nystrom-Lear 2 integrator for a second order ODE.
SecondOrderODEIntegrator *
NystromLear2IntegratorConstructor::create_second_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                NystromLear2SimpleSecondOrderODEIntegrator> (
             size, controls);
}


// Create a Nystrom-Lear 2 integrator for a second order ODE.
SecondOrderODEIntegrator *
NystromLear2IntegratorConstructor::
create_generalized_deriv_second_order_ode_integrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionDerivativeFunctions & deriv_funs,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                NystromLear2GeneralizedDerivSecondOrderODEIntegrator> (
             position_size, velocity_size, deriv_funs, controls);
}

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
