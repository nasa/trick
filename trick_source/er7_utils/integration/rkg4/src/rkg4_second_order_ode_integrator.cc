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
 * Defines member functions for the three fourth order Runge Kutta Gill classes
 * that derive from RK4SecondOrderODEIntegrator:
 * - RKGill4SimpleSecondOrderODEIntegrator,
 * - RKGill4GeneralizedDerivSecondOrderODEIntegrator, and
 * - RKGill4GeneralizedStepSecondOrderODEIntegrator.
 */

/*
Purpose: ()
*/

// System includes

// Interface includes
#include "er7_utils/interface/include/alloc.hh"

// Integration includes
#include "er7_utils/integration/core/include/integ_utils.hh"
#include "er7_utils/integration/core/include/rk_utils.hh"

// Local includes
#include "../include/rkg4_butcher_tableau.hh"
#include "../include/rkg4_second_order_ode_integrator.hh"


namespace er7_utils {


// Clone a RKGill4SimpleSecondOrderODEIntegrator.
RKGill4SimpleSecondOrderODEIntegrator *
RKGill4SimpleSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Clone a RKGill4GeneralizedDerivSecondOrderODEIntegrator.
RKGill4GeneralizedDerivSecondOrderODEIntegrator *
RKGill4GeneralizedDerivSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Clone a RKGill4GeneralizedStepSecondOrderODEIntegrator.
RKGill4GeneralizedStepSecondOrderODEIntegrator *
RKGill4GeneralizedStepSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Propagate state for the special case of velocity being the derivative of
// position.
IntegratorResult
RKGill4SimpleSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   switch (target_stage) {

   // Initial stage (stage 1):
   // Save initial state and update per RKGill4 RKa[1] (one element).
   case 1:
      integ_utils::inplace_two_state_euler_step_save_all (
         accel, RKGill4ButcherTableau::RKa[1][0]*dyn_dt, state_size[0],
         init_pos, init_vel,
         posdot_hist[0], veldot_hist[0],
         position, velocity);
      step_factor = 0.5;
      break;

   // Intermediate stage 2:
   // Update state per RKGill4 RKa[2] (two elements).
   case 2:
      integ_utils::two_state_weighted_step_save_derivs<2> (
         init_pos, init_vel, accel,
         RKGill4ButcherTableau::RKa[2], dyn_dt, state_size[0],
         posdot_hist, veldot_hist, position, velocity);
      step_factor = 0.5;
      break;

   // Intermediate stage 3:
   // Update state per RKGill4 RKa[3] (two elements).
   // Note that RKGill4ButcherTableau::RKa[3][0] is zero.
   case 3:
      integ_utils::two_state_weighted_step_save_derivs<2> (
         init_pos, init_vel, accel,
         RKGill4ButcherTableau::RKa[3]+1, dyn_dt, state_size[0],
         posdot_hist+1, veldot_hist+1, position, velocity);
      step_factor = 1.0;
      break;

   // Final stage (4):
   // Update state per RKGill4 RKb (4 elements).
   case 4:
      integ_utils::two_state_weighted_step<4> (
         init_pos, init_vel, accel, posdot_hist, veldot_hist,
         RKGill4ButcherTableau::RKb, dyn_dt, state_size[0],
         position, velocity);
      step_factor = 1.0;
      break;

   default:
      step_factor = 1.0;
      break;
   }

   return step_factor;
}


// Propagate state for the general case of the generalized position derivative
// being a function of generalized position and generalized velocity.
IntegratorResult
RKGill4GeneralizedDerivSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   // Compute time derivative of canonical position,
   // saving the result in the position derivative history.
   compute_posdot (position, velocity, posdot_hist[target_stage-1]);

   // Advance the state.
   switch (target_stage) {

   // Initial stage (stage 1):
   // Save initial state and update per RKGill4 RKa[1] (one element).
   case 1:
      integ_utils::inplace_two_state_euler_step_save_pos_vel_acc (
         posdot_hist[0], accel,
         RKGill4ButcherTableau::RKa[1][0]*dyn_dt, state_size,
         init_pos, init_vel, veldot_hist[0],
         position, velocity);
      step_factor = 0.5;
      break;

   // Intermediate stage 2:
   // Update state per RKGill4 RKa[2] (two elements).
   case 2:
      integ_utils::two_state_weighted_step_save_veldot<2> (
         init_pos, init_vel, accel,
         RKGill4ButcherTableau::RKa[2], dyn_dt, state_size,
         posdot_hist, veldot_hist, position, velocity);
      step_factor = 0.5;
      break;

   // Intermediate stage 3:
   // Update state per RKGill4 RKa[3] (two elements).
   // Note that RKGill4ButcherTableau::RKa[3][0] is zero.
   case 3:
      integ_utils::two_state_weighted_step_save_veldot<2> (
         init_pos, init_vel, accel,
         RKGill4ButcherTableau::RKa[3]+1, dyn_dt, state_size,
         posdot_hist+1, veldot_hist+1, position, velocity);
      step_factor = 1.0;
      break;

   // Final stage (4):
   // Update state per RKGill4 RKb (4 elements).
   case 4:
      integ_utils::two_state_weighted_step<4> (
         init_pos, init_vel, posdot_hist[3], accel, posdot_hist, veldot_hist,
         RKGill4ButcherTableau::RKb, dyn_dt, state_size,
         position, velocity);
      step_factor = 1.0;
      break;

   default:
      step_factor = 1.0;
      break;
   }

   return step_factor;
}


// Propagate state for the general case of the generalized position derivative
// being a function of generalized position and generalized velocity.
// This doesn't work. I get second order accuracy with both CF and RKMK.
// The code is here, but it won't be used. The constructor uses RK4 instead.
IntegratorResult
RKGill4GeneralizedStepSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   // Advance state per the Munthe-Kaas formulation of Runge-Kutta Gill 4
   switch (target_stage) {
   case 1: {
      double hdt = 0.5*dyn_dt;

      // Save the initial position.
      integ_utils::copy_array (position, state_size[0], init_pos);

      // Compute the dtheta needed for CF4 step 1 and save the input velocity.
      for (int ii = 0, vel_size = state_size[1]; ii < vel_size; ++ii) {
         dtheta[ii] = velocity[ii] * hdt;
         posdot_hist[0][ii] = velocity[ii];
      }

      // Integrate generalized position.
      compute_expmap_position_step (init_pos, dtheta, position);

      // Integrate generalized velocity.
      integ_utils::inplace_euler_step_save_both (
         accel, hdt, state_size[1], init_vel, veldot_hist[0], velocity);

      // Advance time to the midpoint.
      step_factor = 0.5;
      break;
   }

   case 2: {
      // Transform velocity back to the start time.
      compute_dexpinv_velocity_transform (velocity, dtheta, posdot_hist[1]);

      // Integrate generalized position.
      integ_utils::weighted_sum<2> (
         posdot_hist, RKGill4ButcherTableau::RKa[2], dyn_dt, state_size[1],
         dtheta);
      compute_expmap_position_step (init_pos, dtheta, position);

      // Integrate generalized velocity.
      integ_utils::weighted_step_save_deriv<2> (
         init_vel, accel, RKGill4ButcherTableau::RKa[2], dyn_dt, state_size[1],
         veldot_hist, velocity);

      // Advance time to the midpoint.
      step_factor = 0.5;
      break;
   }

   case 3: {
      // Transform velocity back to the start time.
      compute_dexpinv_velocity_transform (velocity, dtheta, posdot_hist[2]);

      // Integrate generalized position.
      integ_utils::weighted_sum<2> (
         posdot_hist, RKGill4ButcherTableau::RKa[3]+1, dyn_dt, state_size[1],
         dtheta);
      compute_expmap_position_step (init_pos, dtheta, position);

      // Integrate generalized velocity.
      integ_utils::weighted_step_save_deriv<2> (
         init_vel, accel, RKGill4ButcherTableau::RKa[3]+1,
         dyn_dt, state_size[1],
         veldot_hist+1, velocity);
      step_factor = 1.0;
      break;
   }

   case 4: {
      // Transform velocity back to the start time.
      compute_dexpinv_velocity_transform (velocity, dtheta, posdot_hist[3]);

      // Integrate generalized position.
      integ_utils::weighted_sum<4> (
         posdot_hist, RKGill4ButcherTableau::RKb, dyn_dt, state_size[1],
         dtheta);
      compute_expmap_position_step (init_pos, dtheta, position);

      // Integrate generalized velocity.
      integ_utils::weighted_step<4> (
          init_vel, accel, veldot_hist,
          RKGill4ButcherTableau::RKb, dyn_dt, state_size[1], velocity);
      step_factor = 1.0;
      break;
   }

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
