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
 * Defines member functions for the class RKFehlberg78IntegratorConstructor.
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
#include "../include/rkf78_integrator_constructor.hh"
#include "../include/rkf78_first_order_ode_integrator.hh"
#include "../include/rkf78_second_order_ode_integrator.hh"


namespace er7_utils {

// Named constructor; create an RKFehlberg78IntegratorConstructor.
IntegratorConstructor*
RKFehlberg78IntegratorConstructor::create_constructor (
   void)
{
   return alloc::allocate_object<RKFehlberg78IntegratorConstructor> ();
}


// Create a duplicate of the constructor.
IntegratorConstructor *
RKFehlberg78IntegratorConstructor::create_copy (
   void)
const
{
   return alloc::replicate_object (*this);
}


// Create an RKFehlberg78 integration controls.
IntegrationControls *
RKFehlberg78IntegratorConstructor::create_integration_controls (
   void)
const
{
   SingleCycleIntegrationControls * controller =
      integ_utils::allocate_controls<SingleCycleIntegrationControls> (13);

// This is a kludge to skip step 10, which isn't needed to calculate
// only the eighth order part of RKF 7/8.
// Also see the source code that implements the state integrators.
#ifdef RKFEHLBERG78_USE_STEP_10
#else
   controller->set_transition_table_element (9, 11);
#endif
   return controller;
}


// Create an RKFehlberg78 state integrator for a first order ODE.
FirstOrderODEIntegrator *
RKFehlberg78IntegratorConstructor::create_first_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<RKFehlberg78FirstOrderODEIntegrator> (
             size, controls);
}


// Create an RKFehlberg78 state integrator for a second order ODE.
SecondOrderODEIntegrator *
RKFehlberg78IntegratorConstructor::create_second_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<RKFehlberg78SimpleSecondOrderODEIntegrator> (
             size, controls);
}


// Create an RKFehlberg78 state integrator for a second order ODE.
SecondOrderODEIntegrator *
RKFehlberg78IntegratorConstructor::
create_generalized_deriv_second_order_ode_integrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionDerivativeFunctions & deriv_funs,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                RKFehlberg78GeneralizedDerivSecondOrderODEIntegrator> (
             position_size, velocity_size, deriv_funs, controls);
}


// Create an RKFehlberg78 state integrator for a second order ODE.
SecondOrderODEIntegrator *
RKFehlberg78IntegratorConstructor::
create_generalized_step_second_order_ode_integrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionStepFunctions & step_funs,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                RKFehlberg78GeneralizedStepSecondOrderODEIntegrator> (
             position_size, velocity_size, step_funs, controls);
}


}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
