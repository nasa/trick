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
 * Defines member functions for class SymplecticEulerSecondOrderODEIntegrator.
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
#include "../include/symplectic_euler_second_order_ode_integrator.hh"


namespace er7_utils {

// SymplecticEulerSecondOrderODEIntegrator default constructor.
SymplecticEulerSecondOrderODEIntegrator::
SymplecticEulerSecondOrderODEIntegrator (
   void)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (),
   posdot (NULL)
{ }


// Copy constructor.
SymplecticEulerSecondOrderODEIntegrator::
SymplecticEulerSecondOrderODEIntegrator (
   const SymplecticEulerSecondOrderODEIntegrator & src)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (src),
   posdot (NULL)
{
   if (problem_type == Integration::GeneralizedStepSecondOrderODE) {
      posdot = alloc::replicate_array (state_size[1], src.posdot);
   }

   else if (problem_type == Integration::GeneralizedDerivSecondOrderODE) {
      posdot = alloc::replicate_array (state_size[0], src.posdot);
   }
}


// SymplecticEulerSecondOrderODEIntegrator non-default constructor for a
// simple second order ODE.
SymplecticEulerSecondOrderODEIntegrator::
SymplecticEulerSecondOrderODEIntegrator (
   unsigned int size,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (size, controls),
   posdot (NULL)
{ }


// SymplecticEulerSecondOrderODEIntegrator non-default constructor for a
// generalized second order ODE in which position is advanced internally using
// the position derivative computed by the provided derivative function.
SymplecticEulerSecondOrderODEIntegrator::
SymplecticEulerSecondOrderODEIntegrator (
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
   // Allocate memory for the position derivative.
   posdot = alloc::allocate_array<double> (position_size);
}


// SymplecticEulerSecondOrderODEIntegrator non-default constructor for a
// generalized second order ODE in which position is advanced externally by the
// provided position step function.
SymplecticEulerSecondOrderODEIntegrator::
SymplecticEulerSecondOrderODEIntegrator (
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
   // Allocate memory for the position derivative.
   posdot = alloc::allocate_array<double> (velocity_size);
}


// SymplecticEulerSecondOrderODEIntegrator destructor.
SymplecticEulerSecondOrderODEIntegrator::
~SymplecticEulerSecondOrderODEIntegrator (
   void)
{
   alloc::deallocate_array (posdot);
}

// Non-throwing swap.
void
SymplecticEulerSecondOrderODEIntegrator::swap (
   SymplecticEulerSecondOrderODEIntegrator & other)
{
   SecondOrderODEIntegrator::swap (other);

   std::swap (posdot, other.posdot);
}


// Clone a SymplecticEulerSimpleSecondOrderODEIntegrator.
SymplecticEulerSimpleSecondOrderODEIntegrator *
SymplecticEulerSimpleSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Clone a SymplecticEulerGeneralizedDerivSecondOrderODEIntegrator.
SymplecticEulerGeneralizedDerivSecondOrderODEIntegrator *
SymplecticEulerGeneralizedDerivSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Clone a SymplecticEulerGeneralizedStepSecondOrderODEIntegrator.
SymplecticEulerGeneralizedStepSecondOrderODEIntegrator *
SymplecticEulerGeneralizedStepSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Propagate state for the special case of velocity being the derivative of
// position.
IntegratorResult
SymplecticEulerSimpleSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   if (target_stage == 1) {

      // Propagate velocity.
      integ_utils::inplace_euler_step (
         accel, dyn_dt, state_size[1], velocity);

      // Propagate position using the propagated velocity.
      integ_utils::inplace_euler_step (
         velocity, dyn_dt, state_size[0], position);
   }

   return 1.0;
}


// Propagate state for the general case of the generalized position derivative
// being a function of generalized position and generalized velocity.
IntegratorResult
SymplecticEulerGeneralizedDerivSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   if (target_stage == 1) {

      // Propagate generalized velocity.
      integ_utils::inplace_euler_step (
         accel, dyn_dt, state_size[1], velocity);

      // Compute position derivative based on propagated velocity
      // and propagate position.
      compute_posdot (position, velocity, posdot);
      integ_utils::inplace_euler_step (
         posdot, dyn_dt, state_size[0], position);
   }

   return 1.0;
}


// Propagate state for the general case of the generalized position derivative
// being a function of generalized position and generalized velocity.
IntegratorResult
SymplecticEulerGeneralizedStepSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   if (target_stage == 1) {

      // Compute a delta theta based on the initial velocity.
      integ_utils::scale_array (velocity, dyn_dt, state_size[1], posdot);

      // Propagate generalized velocity.
      integ_utils::inplace_euler_step (accel, dyn_dt, state_size[1], velocity);

      // Transform the velocity via dexpinv using the above delta theta.
      compute_dexpinv_velocity_transform (velocity, posdot, posdot);

      // Advance position based on the input position and propagated velocity.
      integ_utils::scale_array (posdot, dyn_dt, state_size[1], posdot);
      compute_expmap_position_step (position, posdot, position);
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
