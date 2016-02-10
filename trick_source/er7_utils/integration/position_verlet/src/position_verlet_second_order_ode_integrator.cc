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
 * Defines member functions for the class
 * PositionVerletSecondOrderODEIntegrator.
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
#include "../include/position_verlet_second_order_ode_integrator.hh"


namespace er7_utils {

// PositionVerletSecondOrderODEIntegrator default constructor.
PositionVerletSecondOrderODEIntegrator::PositionVerletSecondOrderODEIntegrator (
   void)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (),
   posdot (NULL),
   dtheta (NULL),
   h_dtheta (NULL),
   midpoint (NULL),
   init_vel (NULL),
   prev_accel (NULL),
   restart (true)
{ }


// Copy constructor.
PositionVerletSecondOrderODEIntegrator::PositionVerletSecondOrderODEIntegrator (
   const PositionVerletSecondOrderODEIntegrator & src)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (src),
   posdot (NULL),
   dtheta (NULL),
   h_dtheta (NULL),
   midpoint (NULL),
   init_vel (NULL),
   prev_accel (NULL),
   restart (src.restart)
{
   // Replicate the source's contents if they exist.
   if (problem_type == Integration::GeneralizedStepSecondOrderODE) {
      dtheta = alloc::replicate_array (state_size[1], src.dtheta);
      h_dtheta = alloc::replicate_array (state_size[1], src.h_dtheta);
      midpoint = alloc::replicate_array (state_size[0], src.midpoint);
      init_vel = alloc::replicate_array (state_size[1], src.init_vel);
      prev_accel = alloc::replicate_array (state_size[1], src.prev_accel);
   }

   else if (problem_type == Integration::GeneralizedDerivSecondOrderODE) {
      posdot = alloc::replicate_array (state_size[0], src.posdot);
      init_vel = alloc::replicate_array (state_size[1], src.init_vel);
      prev_accel = alloc::replicate_array (state_size[1], src.prev_accel);
   }

   else if (src.init_vel != NULL) {
      init_vel = alloc::replicate_array (state_size[1], src.init_vel);
      prev_accel = alloc::replicate_array (state_size[1], src.prev_accel);
   }
}


// Non-default constructor for a PositionVerletSecondOrderODEIntegrator
// in which the time derivative of position is the generalized velocity.
PositionVerletSecondOrderODEIntegrator::PositionVerletSecondOrderODEIntegrator (
   unsigned int size,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (size, controls),
   posdot (NULL),
   dtheta (NULL),
   h_dtheta (NULL),
   midpoint (NULL),
   init_vel (NULL),
   prev_accel (NULL),
   restart (true)
{
   init_vel = alloc::allocate_array<double> (size);
   prev_accel = alloc::allocate_array<double> (size);
}


// Non-default constructor for a PositionVerletSecondOrderODEIntegrator
// for generalized position, generalized velocity.
PositionVerletSecondOrderODEIntegrator::PositionVerletSecondOrderODEIntegrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionDerivativeFunctions & deriv_funs,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (position_size, velocity_size,
                             deriv_funs, controls),
   posdot (NULL),
   dtheta (NULL),
   h_dtheta (NULL),
   midpoint (NULL),
   init_vel (NULL),
   prev_accel (NULL),
   restart (true)
{
   // Allocate memory used by Nystrom Lear 2 algorithm.
   posdot = alloc::allocate_array<double> (position_size);
   init_vel = alloc::allocate_array<double> (velocity_size);
   prev_accel = alloc::allocate_array<double> (velocity_size);
}


// Non-default constructor for a PositionVerletSecondOrderODEIntegrator
// for generalized position, generalized velocity.
PositionVerletSecondOrderODEIntegrator::PositionVerletSecondOrderODEIntegrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionStepFunctions & step_funs,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (position_size, velocity_size,
                             step_funs, controls),
   posdot (NULL),
   dtheta (NULL),
   h_dtheta (NULL),
   midpoint (NULL),
   init_vel (NULL),
   prev_accel (NULL),
   restart (true)
{
   // Allocate memory used by Nystrom Lear 2 algorithm.
   dtheta = alloc::allocate_array<double> (velocity_size);
   h_dtheta = alloc::allocate_array<double> (velocity_size);
   midpoint = alloc::allocate_array<double> (position_size);
   init_vel = alloc::allocate_array<double> (velocity_size);
   prev_accel = alloc::allocate_array<double> (velocity_size);
}


// PositionVerletSecondOrderODEIntegrator destructor.
PositionVerletSecondOrderODEIntegrator::~PositionVerletSecondOrderODEIntegrator (
   void)
{
   alloc::deallocate_array<double> (posdot);
   alloc::deallocate_array<double> (dtheta);
   alloc::deallocate_array<double> (h_dtheta);
   alloc::deallocate_array<double> (midpoint);
   alloc::deallocate_array<double> (init_vel);
   alloc::deallocate_array<double> (prev_accel);
}


// Non-throwing swap.
void
PositionVerletSecondOrderODEIntegrator::swap (
   PositionVerletSecondOrderODEIntegrator & other)
{
   SecondOrderODEIntegrator::swap (other);

   std::swap (posdot, other.posdot);
   std::swap (dtheta, other.dtheta);
   std::swap (h_dtheta, other.h_dtheta);
   std::swap (midpoint, other.midpoint);
   std::swap (init_vel, other.init_vel);
   std::swap (prev_accel, other.prev_accel);
}


// Clone a PositionVerletSimpleSecondOrderODEIntegrator.
PositionVerletSimpleSecondOrderODEIntegrator *
PositionVerletSimpleSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Clone a PositionVerletGeneralizedDerivSecondOrderODEIntegrator.
PositionVerletGeneralizedDerivSecondOrderODEIntegrator *
PositionVerletGeneralizedDerivSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Clone a PositionVerletGeneralizedStepSecondOrderODEIntegrator.
PositionVerletGeneralizedStepSecondOrderODEIntegrator *
PositionVerletGeneralizedStepSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


/**
 * Nystrom-Lear 2 step #2 for the special case where the derivative of position
 * is the generalized velocity.
 * @param[in]     init_vel     Initial velocity
 * @param[in]     accel        Acceleration
 * @param[in]     deltat       Time step
 * @param[in]     size         State size
 * @param[in,out] position     Updated position vector
 * @param[in,out] velocity     Updated velocity vector
 * @param[out]    saved_acc    Saved acceleration vector
 */
inline void ER7_UTILS_ALWAYS_INLINE
position_verlet_step2 (
   double const * ER7_UTILS_RESTRICT init_vel,
   double const * ER7_UTILS_RESTRICT accel,
   double deltat,
   int size,
   double * ER7_UTILS_RESTRICT position,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT saved_acc)
{
   double hdt = 0.5*deltat;
   for (int ii = 0; ii < size; ++ii) {
      saved_acc[ii] = accel[ii];
      velocity[ii]  = init_vel[ii] + accel[ii] * deltat;
      position[ii] += velocity[ii] * hdt;
   }
}


// Propagate state for the special case of velocity being the derivative of
// position.
IntegratorResult
PositionVerletSimpleSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   switch (target_stage) {

   // Canonical position verlet / position verlet step #1 advances position to
   // the middle of the integration interval but leaves velocity unchanged.
   // This behavior leads to suboptimal performance when acceleration
   // is a function of position and velocity. So also advance velocity.
   // Since acceleration is only computed on the first step on resets,
   // we need to use the saved accel to advance velocity (except on resets).
   case 1: {
      double hdt = 0.5*dyn_dt;

      // Advance position to the midpoint using the initial velocity.
      integ_utils::inplace_euler_step (
         velocity, hdt, state_size[0], position);

      // Advance velocity to the midpoint using the current accel
      // on restarts, previous accel otherwise.
      if (restart) {
         restart = false;

         integ_utils::inplace_euler_step_save_state (
            accel, hdt, state_size[1], init_vel, velocity);
      }
      else {
         integ_utils::inplace_euler_step_save_state (
            prev_accel, hdt, state_size[1], init_vel, velocity);
      }

      step_factor = 0.5;
      break;
   }

   // Step #2 advances velocity a full step, then
   // advances position a half step (we're already at the midpoint) using
   // the end velocity.
   case 2:
      position_verlet_step2 (init_vel, accel, dyn_dt, state_size[0],
                 position, velocity, prev_accel);
      step_factor = 1.0;
      break;

   default:
      step_factor = 1.0;
      break;
   }

   return step_factor;
}


// Propagate state via position verlet for generalized position and generalized
// velocity where generalized position is advanced using the function that
// yields the time derivative of generalized position.
IntegratorResult
PositionVerletGeneralizedDerivSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   switch (target_stage) {

   // Advance state to the middle of the integration interval.
   // See commentary in the simple position verlet integrator above for details.
   case 1: {
      double hdt = 0.5*dyn_dt;

      // Advance position to the midpoint using the current velocity.
      compute_posdot (position, velocity, posdot);
      integ_utils::inplace_euler_step (
         posdot, hdt, state_size[0], position);

      // Advance velocity to the midpoint using the current accel
      // on restarts, previous accel otherwise.
      if (restart) {
         restart = false;

         integ_utils::inplace_euler_step_save_state (
            accel, hdt, state_size[1], init_vel, velocity);
      }
      else {
         integ_utils::inplace_euler_step_save_state (
            prev_accel, hdt, state_size[1], init_vel, velocity);
      }

      step_factor = 0.5;
      break;
   }

   // Advance velocity as a full Euler step, the advance position a
   // half step using the end velocity.
   case 2:
      integ_utils::euler_step_save_deriv (
         init_vel, accel, dyn_dt, state_size[1], prev_accel, velocity);

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


// Propagate state via position verlet for generalized position and generalized
// velocity where generalized position is advanced using the position step
// function.
IntegratorResult
PositionVerletGeneralizedStepSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   switch (target_stage) {

   // Canonical position verlet / position verlet: Advance position to the
   // middle of the integration interval but leave velocity unchanged.
   // This nominal behavior leads to suboptimal performance when acceleration
   // is a function of position and velocity. So also advance velocity.
   // Since acceleration is only computed on the first step on resets,
   // we need to use the saved accel to advance velocity (except on resets).
   case 1:
      integ_utils::copy_array (velocity, state_size[1], init_vel);

      if (restart) {
         restart = false;

         // Use the current velocity to compute a position advance,
         // then advance position.
         integ_utils::scale_array (
            velocity, 0.5*dyn_dt, state_size[1], h_dtheta);
         compute_expmap_position_step (position, h_dtheta, midpoint);

         // Integrate generalized velocity.
         integ_utils::inplace_euler_step (
            accel, 0.5*dyn_dt, state_size[1], velocity);
      }

      // Nominal case: Advance position from the previous midpoint,
      // velocity with the saved accelerations.
      else {

         // Propagate the previous midpoint position to the middle
         // of the current integration interval.
         compute_expmap_position_step (midpoint, dtheta, midpoint);

         // Propagate velocity using the saved acceleration.
         integ_utils::inplace_euler_step (
            prev_accel, 0.5*dyn_dt, state_size[1], velocity);
      }

      integ_utils::copy_array (midpoint, state_size[0], position);

      step_factor = 0.5;
      break;

   // Advance velocity a full step, then advance position a half step
   // using the end velocity.
   case 2:

      // Integrate generalized velocity, saving the accel for the next step.
      integ_utils::euler_step_save_deriv (
         init_vel, accel, dyn_dt, state_size[1], prev_accel, velocity);

      // Compute dtheta = velocity*dyn_dt, h_dtheta = dtheta/2
      // Note: Transforming velocity to the midpoint via dexpinv
      // gives notably worse performance.
      for (int ii = 0, lim = state_size[1]; ii < lim; ++ii) {
         dtheta[ii] = velocity[ii]*dyn_dt;
         h_dtheta[ii] = 0.5*dtheta[ii];
      }

      // Integrate generalized position.
      compute_expmap_position_step (position, h_dtheta, position);

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
