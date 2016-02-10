/*******************************************************************************

Purpose:
  (Define member functions for the class RK4IntegratorConstructor.)

Library dependencies:
  ((rkn4_integrator_constructor.o)
   (rkn4_second_order_ode_integrator.o)
   (er7_utils/integration/rk4/rk4_first_order_ode_integrator.o)
   (er7_utils/integration/rk4/rk4_second_order_ode_integrator.o)
   (er7_utils/integration/core/single_cycle_integration_controls.o)
   (er7_utils/integration/core/integrator_constructor.o))

Programmers:
  (((David Hammen) (OSR) (July 2008) (JEOD_2.0) (Initial version))
   ((David Hammen) (OSR) (March 2009) (JEOD_2.0 #22) (Cleanup))
   ((David Hammen) (OSR) (Sept 2011) (JEOD 2.2 #537)
    (Multi-cycle support)))

*******************************************************************************/

// System includes

// Interface includes
#include "er7_utils/interface/include/alloc.hh"

// Integration includes
#include "er7_utils/integration/core/include/integrator_constructor_utils.hh"
#include "er7_utils/integration/core/include/single_cycle_integration_controls.hh"
#include "er7_utils/integration/rk4/include/rk4_first_order_ode_integrator.hh"
#include "er7_utils/integration/rk4/include/rk4_second_order_ode_integrator.hh"

// Model includes
#include "../include/rkn4_integrator_constructor.hh"
#include "../include/rkn4_second_order_ode_integrator.hh"


namespace er7_utils {

// Named constructor; create an RKNystrom4IntegratorConstructor.
IntegratorConstructor*
RKNystrom4IntegratorConstructor::create_constructor (
   void)
{
   return alloc::allocate_object<RKNystrom4IntegratorConstructor> ();
}


// Create a duplicate of the constructor.
IntegratorConstructor *
RKNystrom4IntegratorConstructor::create_copy (
   void)
const
{
   return alloc::replicate_object (*this);
}


// Create an RKNystrom4 integration controls.
IntegrationControls *
RKNystrom4IntegratorConstructor::create_integration_controls (
   void)
const
{
   return integ_utils::allocate_controls<SingleCycleIntegrationControls> (4);
}


// Create an RK4 state integrator as a surrogate for a first order ODE.
FirstOrderODEIntegrator *
RKNystrom4IntegratorConstructor::create_first_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<RK4FirstOrderODEIntegrator> (
             size, controls);
}


// Create a 4th order Runge Kutta Nystrom state integrator for a
// simple second order ODE.
SecondOrderODEIntegrator *
RKNystrom4IntegratorConstructor::create_second_order_ode_integrator (
   unsigned int size,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                RKNystrom4SimpleSecondOrderODEIntegrator> (
             size, controls);
}


// Create an RK4 state integrator as a surrogate for a second order ODE
// in which position is advanced with the position derivative functions.
SecondOrderODEIntegrator *
RKNystrom4IntegratorConstructor::
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


// Create a 4th order Runge Kutta Nystrom state integrator for a second order
// ODE in which position is constrained to lie on a manifold.
SecondOrderODEIntegrator *
RKNystrom4IntegratorConstructor::
create_generalized_step_second_order_ode_integrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionStepFunctions & step_funs,
   IntegrationControls & controls)
const
{
   return integ_utils::allocate_integrator<
                RKNystrom4GeneralizedStepSecondOrderODEIntegrator> (
             position_size, velocity_size, step_funs, controls);
}


}
