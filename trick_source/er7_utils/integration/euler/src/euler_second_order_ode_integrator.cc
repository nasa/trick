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
 * Defines member functions for the class EulerSecondOrderODEIntegrator.
 */

/*
Purpose: ()
*/

// System includes

// Interface includes
#include "er7_utils/interface/include/alloc.hh"

// Integration includes
#include "er7_utils/integration/core/include/integ_utils.hh"

// Model includes
#include "../include/euler_second_order_ode_integrator.hh"


namespace er7_utils {

// EulerSecondOrderODEIntegrator default constructor.
EulerSecondOrderODEIntegrator::EulerSecondOrderODEIntegrator (
   void)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (),
   posdot (NULL)
{ }


// Copy constructor.
EulerSecondOrderODEIntegrator::EulerSecondOrderODEIntegrator (
   const EulerSecondOrderODEIntegrator & src)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (src),
   posdot (NULL)
{
   // Replicate the source's contents if they exist.
   if (problem_type == Integration::GeneralizedStepSecondOrderODE) {
      posdot = alloc::replicate_array (state_size[1], src.posdot);
   }

   else if (problem_type == Integration::GeneralizedDerivSecondOrderODE) {
      posdot = alloc::replicate_array (state_size[0], src.posdot);
   }
}



// EulerSecondOrderODEIntegrator non-default constructor for a simple
// second order ODE.
EulerSecondOrderODEIntegrator::EulerSecondOrderODEIntegrator (
   unsigned int size,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (size, controls),
   posdot (NULL)
{ }


// EulerSecondOrderODEIntegrator non-default constructor for a generalized
// second order ODE in which position is advanced internally using the position
// derivative computed by the provided derivative function.
EulerSecondOrderODEIntegrator::EulerSecondOrderODEIntegrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionDerivativeFunctions & deriv_funs,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (position_size, velocity_size,
                             deriv_funs, controls),
   posdot (NULL)
{
   // Allocate memory used by Euler integration.
   posdot = alloc::allocate_array<double> (position_size);
}


// EulerSecondOrderODEIntegrator non-default constructor for a generalized
// second order ODE in which position is advanced externally by the provided
// position step function.
EulerSecondOrderODEIntegrator::EulerSecondOrderODEIntegrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionStepFunctions & step_funs,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (position_size, velocity_size,
                             step_funs, controls),
   posdot (NULL)
{
   // Allocate memory used by Euler integration.
   posdot = alloc::allocate_array<double> (velocity_size);
}


// EulerSecondOrderODEIntegrator destructor.
EulerSecondOrderODEIntegrator::~EulerSecondOrderODEIntegrator (
   void)
{
   alloc::deallocate_array<double> (posdot);
}


// Non-throwing swap.
void
EulerSecondOrderODEIntegrator::swap (
   EulerSecondOrderODEIntegrator & other)
{
   SecondOrderODEIntegrator::swap (other);

   std::swap (posdot, other.posdot);
}


// Clone a EulerSimpleSecondOrderODEIntegrator.
EulerSimpleSecondOrderODEIntegrator *
EulerSimpleSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Clone a EulerGeneralizedDerivSecondOrderODEIntegrator.
EulerGeneralizedDerivSecondOrderODEIntegrator *
EulerGeneralizedDerivSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Clone a EulerGeneralizedStepSecondOrderODEIntegrator.
EulerGeneralizedStepSecondOrderODEIntegrator *
EulerGeneralizedStepSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Propagate state for the special case of velocity being the derivative of
// position.
IntegratorResult
EulerSimpleSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   if (target_stage == 1) {

      // Propagate position using the propagated velocity.
      integ_utils::inplace_euler_step (velocity, dyn_dt, state_size[0],
                                       position);

      // Propagate velocity.
      integ_utils::inplace_euler_step (accel, dyn_dt, state_size[1],
                                       velocity);
   }

   return 1.0;
}


// Propagate state for the general case of the generalized position derivative
// being a function of generalized position and generalized velocity.
IntegratorResult
EulerGeneralizedDerivSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   if (target_stage == 1) {

      // Compute position derivative based on the input velocity
      // and propagate position.
      compute_posdot (position, velocity, posdot);
      integ_utils::inplace_euler_step (posdot, dyn_dt, state_size[0],
                                       position);

      // Propagate generalized velocity.
      integ_utils::inplace_euler_step (accel, dyn_dt, state_size[1],
                                       velocity);
   }

   return 1.0;
}


// Propagate state for the general case of the generalized position derivative
// being a function of generalized position and generalized velocity.
IntegratorResult
EulerGeneralizedStepSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   if (target_stage == 1) {

      // Propagate position using the external step function.
      integ_utils::scale_array (velocity, dyn_dt, state_size[1], posdot);
      compute_expmap_position_step (position, posdot, position);

      // Propagate generalized velocity as an Euler step.
      integ_utils::inplace_euler_step (accel, dyn_dt, state_size[1],
                                       velocity);
   }

   return 1.0;
}

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
