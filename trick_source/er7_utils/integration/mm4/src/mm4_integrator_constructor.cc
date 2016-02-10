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
 * Defines member functions for the class MM4IntegratorConstructor.
 */

/*
Purpose: ()
*/

// System includes

// Interface includes
#include "er7_utils/interface/include/alloc.hh"

// Model includes
#include "er7_utils/integration/core/include/integrator_constructor_utils.hh"
#include "er7_utils/integration/core/include/single_cycle_integration_controls.hh"
#include "er7_utils/integration/rk2_midpoint/include/rk2_midpoint_first_order_ode_integrator.hh"

// Local includes
#include "../include/mm4_integrator_constructor.hh"
#include "../include/mm4_second_order_ode_integrator.hh"


namespace er7_utils {

// Named constructor; create an MM4IntegratorConstructor.
IntegratorConstructor*
MM4IntegratorConstructor::create_constructor (
   void)
{
   return alloc::allocate_object<MM4IntegratorConstructor> ();
}


// Create a duplicate of the constructor.
IntegratorConstructor *
MM4IntegratorConstructor::create_copy (
   void)
const
{
   return alloc::replicate_object (*this);
}


// Create an MM4 integration controls.
IntegrationControls *
MM4IntegratorConstructor::create_integration_controls (
   void)
const
{
   return integ_utils::allocate_controls<SingleCycleIntegrationControls> (3);
}


// Create an MM4-compatible state integrator for a first order ODE.
FirstOrderODEIntegrator *
MM4IntegratorConstructor::create_first_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<RK2MidpointFirstOrderODEIntegrator> (
             size, controls);
}


// Create an MM4 state integrator for a second order ODE.
SecondOrderODEIntegrator *
MM4IntegratorConstructor::create_second_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<MM4SimpleSecondOrderODEIntegrator> (
             size, controls);
}


// Create an MM4 state integrator for a second order ODE.
SecondOrderODEIntegrator *
MM4IntegratorConstructor::
create_generalized_deriv_second_order_ode_integrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionDerivativeFunctions & deriv_funs,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                MM4GeneralizedDerivSecondOrderODEIntegrator> (
             position_size, velocity_size, deriv_funs, controls);
}

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
