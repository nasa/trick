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
 * Defines member functions for the class NystromLear2SecondOrderODEIntegrator.
 */

/*
Purpose: ()
*/

// System includes
#include <algorithm>
#include <cstddef>

// Interface includes
#include "er7_utils/interface/include/alloc.hh"

// Integration includes
#include "er7_utils/integration/core/include/integ_utils.hh"

// Model includes
#include "../include/nl2_second_order_ode_integrator.hh"


namespace er7_utils {

// NystromLear2SecondOrderODEIntegrator default constructor.
NystromLear2SecondOrderODEIntegrator::NystromLear2SecondOrderODEIntegrator (
   void)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (),
   posdot (NULL)
{ }


// Copy constructor.
NystromLear2SecondOrderODEIntegrator::NystromLear2SecondOrderODEIntegrator (
   const NystromLear2SecondOrderODEIntegrator & src)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (src),
   posdot (NULL)
{
   // Replicate the source's contents if they exist.
   if (problem_type == Integration::GeneralizedDerivSecondOrderODE) {
      posdot = alloc::replicate_array (state_size[0], src.posdot);
   }
}


// Non-default constructor for a NystromLear2SecondOrderODEIntegrator
// in which the time derivative of position is the generalized velocity.
NystromLear2SecondOrderODEIntegrator::NystromLear2SecondOrderODEIntegrator (
   unsigned int size,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (size, controls),
   posdot (NULL)
{ }


// Non-default constructor for a NystromLear2SecondOrderODEIntegrator
// for generalized position, generalized velocity.
NystromLear2SecondOrderODEIntegrator::NystromLear2SecondOrderODEIntegrator (
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
   // Allocate memory used by Nystrom Lear 2 algorithm.
   posdot = alloc::allocate_array<double> (position_size);
}


// NystromLear2SecondOrderODEIntegrator destructor.
NystromLear2SecondOrderODEIntegrator::~NystromLear2SecondOrderODEIntegrator (
   void)
{
   alloc::deallocate_array<double> (posdot);
}


// Non-throwing swap.
void
NystromLear2SecondOrderODEIntegrator::swap (
   NystromLear2SecondOrderODEIntegrator & other)
{
   SecondOrderODEIntegrator::swap (other);

   std::swap (posdot, other.posdot);
}


// Clone a NystromLear2SimpleSecondOrderODEIntegrator.
NystromLear2SimpleSecondOrderODEIntegrator *
NystromLear2SimpleSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Clone a NystromLear2GeneralizedDerivSecondOrderODEIntegrator.
NystromLear2GeneralizedDerivSecondOrderODEIntegrator *
NystromLear2GeneralizedDerivSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


/**
 * Nystrom-Lear 2 step #2 for the special case where the derivative of position
 * is the generalized velocity.
 * @param[in]     accel        Acceleration
 * @param[in]     deltat       Time step
 * @param[in]     size         State size
 * @param[in,out] position     Updated position vector
 * @param[in,out] velocity     Updated velocity vector
 */
inline void ER7_UTILS_ALWAYS_INLINE
nl2_step2 (
   double const * ER7_UTILS_RESTRICT accel,
   double deltat,
   int size,
   double * ER7_UTILS_RESTRICT position,
   double * ER7_UTILS_RESTRICT velocity)
{
   double hdt = 0.5*deltat;
   for (int ii = 0; ii < size; ++ii) {
      velocity[ii] += accel[ii] * deltat;
      position[ii] += velocity[ii] * hdt;
   }
}


// Propagate state for the special case of velocity being the derivative of
// position.
IntegratorResult
NystromLear2SimpleSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   switch (target_stage) {

   // Advance position to the midpoint, leave velocity unchanged.
   case 1: {
      double hdt = 0.5*dyn_dt;

      // Advance position to the midpoint using the initial velocity.
      integ_utils::inplace_euler_step (
         velocity, hdt, state_size[0], position);

      step_factor = 0.5;
      break;
   }

   // Step #2 advances velocity a full step, then
   // advances position a half step (we're already at the midpoint) using
   // the end velocity.
   case 2:
      nl2_step2 (accel, dyn_dt, state_size[0], position, velocity);
      step_factor = 1.0;
      break;

   default:
      step_factor = 1.0;
      break;
   }

   return step_factor;
}


// Propagate state via NL2 for generalized position and generalized velocity
// where generalized position is advanced using the function that yields
// the time derivative of generalized position.
IntegratorResult
NystromLear2GeneralizedDerivSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   switch (target_stage) {

   // Advance position to the middle of the integration interval.
   case 1:

      // Advance position to the midpoint using the current velocity.
      compute_posdot (position, velocity, posdot);
      integ_utils::inplace_euler_step (
         posdot, 0.5*dyn_dt, state_size[0], position);

      step_factor = 0.5;
      break;

   // Advance velocity as a full Euler step using the end acceleration,
   // then advance position a half step using the end velocity.
   case 2:
      integ_utils::inplace_euler_step (
         accel, dyn_dt, state_size[1], velocity);

      compute_posdot (position, velocity, posdot);
      integ_utils::inplace_euler_step (
         posdot, 0.5*dyn_dt, state_size[0], position);

      step_factor = 1.0;
      break;

   default:
      step_factor = 1.0;
      break;
   }

   return step_factor;
}

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
