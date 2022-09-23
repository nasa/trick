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
 * Defines member functions for the three RK4 classes that derive from
 * RK4SecondOrderODEIntegrator:
 * - RK4SimpleSecondOrderODEIntegrator,
 * - RK4GeneralizedDerivSecondOrderODEIntegrator, and
 * - RK4GeneralizedStepSecondOrderODEIntegrator.
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

// Model includes
#include "../include/rk4_second_order_ode_integrator.hh"


namespace er7_utils {

// Clone a RK4SimpleSecondOrderODEIntegrator.
RK4SimpleSecondOrderODEIntegrator *
RK4SimpleSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Clone a RK4GeneralizedDerivSecondOrderODEIntegrator.
RK4GeneralizedDerivSecondOrderODEIntegrator *
RK4GeneralizedDerivSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Clone a RK4GeneralizedStepSecondOrderODEIntegrator.
RK4GeneralizedStepSecondOrderODEIntegrator *
RK4GeneralizedStepSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Propagate state for the special case of velocity being the derivative of
// position.
IntegratorResult
RK4SimpleSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   /**
    * ### Overview
    *
    * This method applies the canonical fourth order Runga Kutta technique
    * to a simple second order ODE. One integration cycle comprises four
    * intermediate steps that operate per the following Butcher tableau:
    * @code
    *  0  |
    * 1/2 | 1/2
    * 1/2 |  0  1/2
    *  1  |  0   0   1
    * ----+----------------
    *  1  | 1/6 1/3 1/3 1/6
    * @endcode
    *
    * The 4th order Runga Kutta Butcher tableau is of such a simple form
    * that a tableau-based formulation is not used. The code instead
    * implements the tableau directly.
    *
    * ### Algorithm
    *
    * The input target stage (1 to 4) specifies the RK4 intermediate step.
    * A target_stage based switch statement directs the computation.
    */

   // Advance state per RK4.
   switch (target_stage) {

   /**
    * - First intermediate step:@n
    *   Advance state (position and velocity) to the midpoint of the
    *   integration interval as a simple Euler half step, saving the
    *   initial position and velocity for use in subsequent stages.
    */
   case 1:
      integ_utils::inplace_two_state_euler_step_save_all (
         accel, 0.5*dyn_dt, state_size[0],
         init_pos, init_vel,
         posdot_hist[0], veldot_hist[0], position, velocity);
      step_factor = 0.5;
      break;

   /**
    * - Second intermediate step:@n
    *   Advance the initial state (position and velocity) to the midpoint of
    *   the integration interval as a simple Euler half step, but using
    *   accelerations calculated based on the state obtained from the
    *   first intermediate step.
    */
   case 2:
      rk::rk_two_state_intermediate_step (
         init_pos, init_vel, accel, 0.5*dyn_dt, state_size[0],
         posdot_hist[1], veldot_hist[1], position, velocity);
      step_factor = 0.5;
      break;

   /**
    * - Third intermediate step:@n
    *   Advance the initial state (position and velocity) to the end of
    *   the integration interval as a simple Euler full step, but using
    *   accelerations calculated based on the state obtained from the
    *   second intermediate step.
    */
   case 3:
      rk::rk_two_state_intermediate_step (
         init_pos, init_vel, accel, dyn_dt, state_size[0],
         posdot_hist[2], veldot_hist[2], position, velocity);
      step_factor = 1.0;
      break;

   /**
    * - Final step:@n
    *   Advance the initial state (position and velocity) to the end of
    *   the integration interval as a weighted Euler step, using the Butcher
    *   tableau 'b' vector (1/6, 1/3, 1/3, 1/6) to form a weighted velocity
    *   to propagate position, weighted acceleration to propagate velocity.
    */
   case 4:
      rk::rk4_two_state_final_step (
         init_pos, init_vel, posdot_hist, veldot_hist, accel,
         dyn_dt, state_size[0],
         position, velocity);
      step_factor = 1.0;
      break;

   default:
      step_factor = 1.0;
      break;
   }

   return step_factor;
}


// Propagate state via RK4 for generalized position and generalized velocity
// where generalized position is advanced using the function that yields
// the time derivative of generalized position.
IntegratorResult
RK4GeneralizedDerivSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   /**
    * ### Overview
    *
    * This method adapts the canonical fourth order Runga Kutta technique
    * to a generalized second order ODE in which generalized position is
    * advanced using the function compute_posdot that yields the time
    * derivative of generalized position.
    * See RK4SimpleSecondOrderODEIntegrator::integrate for a description
    * of and details on the RK4 algorithm.
    *
    * ### Algorithm
    */


   /**
    * Prior to entering the standard switch on the target_stage,
    * compute time derivative of canonical position, saving the result in
    * the appropriate slot in position derivative history.
    */
   compute_posdot (position, velocity, posdot_hist[target_stage-1]);

   /**
    * A target_stage based switch statement directs the bulk of the
    * computations. The staging is very similar to that of the
    * simple second order RK4 integrator, but here the technique treats
    * generalized position and generalized velocity separately.
    */
   switch (target_stage) {
   case 1: {
      double hdt = 0.5*dyn_dt;
      integ_utils::inplace_euler_step_save_state (
         posdot_hist[0], hdt, state_size[0], init_pos, position);
      integ_utils::inplace_euler_step_save_both (
         accel, hdt, state_size[1], init_vel, veldot_hist[0], velocity);
      step_factor = 0.5;
      break;
   }

   case 2: {
      double hdt = 0.5*dyn_dt;
      integ_utils::euler_step (
         init_pos, posdot_hist[1], hdt, state_size[0], position);
      integ_utils::euler_step_save_deriv (
         init_vel, accel, hdt, state_size[1], veldot_hist[1], velocity);
      step_factor = 0.5;
      break;
   }

   case 3:
      integ_utils::euler_step (
         init_pos, posdot_hist[2], dyn_dt, state_size[0], position);
      integ_utils::euler_step_save_deriv (
         init_vel, accel, dyn_dt, state_size[1], veldot_hist[2], velocity);
      step_factor = 1.0;
      break;

   case 4:
      rk::rk4_final_step (
         init_pos, posdot_hist, posdot_hist[3], dyn_dt, state_size[0],
         position);
      rk::rk4_final_step (
         init_vel, veldot_hist, accel, dyn_dt, state_size[1],
         velocity);
      step_factor = 1.0;
      break;

   default:
      step_factor = 1.0;
      break;
   }

   return step_factor;
}


// Propagate state via RK4 for generalized position and generalized velocity
// where generalized position is advanced using the position step function.
IntegratorResult
RK4GeneralizedStepSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   /**
    * ### Overview
    *
    * This method adapts the canonical fourth order Runga Kutta technique
    * to a generalized second order ODE in which generalized position is
    * a member of a Lie group and generalized velocity is a vectorized member
    * of the corresponding Lie algebra. The canonical example is orientation
    * and angular velocity in three space.
    *
    * Three distinct techniques were evaluated in the formulation of this
    * method:
    *  - Crouch-Grossman (CG) integration, which makes seven calls to the
    *    exponential map function per integration cycle;
    *  - Runge-Kutta Munthe-Kaas (RKMK) integration, which makes only four calls
    *    to the exponential map function per integration cycle at the expense of
    *    three calls to the dexpinv function; and
    *  - A commutator-free integrator, which makes five calls to the
    *    exponential map function per integration cycle.
    *
    * On the test problems used to evaluate integrators, all three approaches
    * performed much better in terms of error than did the corresponding
    * generalized derivative integration. The commutator-free technique
    * consistently did better than both Crouch-Grossman and
    * Runge-Kutta Munthe-Kaas.
    *
    * ### Algorithm
    *
    * The input target stage (1 to 4) specifies the RK4 intermediate step.
    * A target_stage based switch statement directs the computation.
    */

   // Advance state per commutator-free RK4.
   switch (target_stage) {

   /**
    * - First intermediate step:@n
    *   Advance state to the midpoint of the integration interval
    *   using the input velocity and acceleration as the basis for
    *   the propagation.
    *   Generalized position is propagated via the exponential map function
    *   operating on v*(dt/2) while generalized velocity is propagated
    *   as a simple Euler half step.@n
    *   The following items are saved for use on subsequent steps:
    *   + Initial position in init_pos,
    *   + Initial velocity in posdot_hist[0] and in init_vel,
    *   + Initial acceleration in veldot_hist[0], and
    *   + Propagated position in saved_pos.
    */
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

      // Save the integrated position for reuse in step 3.
      integ_utils::copy_array (position, state_size[0], saved_pos);

      // Integrate generalized velocity.
      integ_utils::inplace_euler_step_save_both (
         accel, hdt, state_size[1], init_vel, veldot_hist[0], velocity);

      // Advance time to the midpoint.
      step_factor = 0.5;
      break;
   }

   /**
    * - Second intermediate step:@n
    *   Advance state to the midpoint of the integration interval
    *   using the input velocity and acceleration as the basis for
    *   the propagation.
    *   Generalized position is propagated via the exponential map function
    *   operating on v*(dt/2) while generalized velocity is propagated
    *   as a simple Euler half step.@n
    *   The input velocity and acceleration are saved in posdot_hist[1]
    *   and veldot_hist[1] for use on subsequent steps.
    */
   case 2: {
      double hdt = 0.5*dyn_dt;

      // Compute the dtheta needed for CF4 step 2 and save the input velocity.
      for (int ii = 0, vel_size = state_size[1]; ii < vel_size; ++ii) {
         dtheta[ii] = velocity[ii] * hdt;
         posdot_hist[1][ii] = velocity[ii];
      }

      // Integrate generalized position.
      compute_expmap_position_step (init_pos, dtheta, position);

      // Integrate generalized velocity.
      integ_utils::euler_step_save_deriv (
         init_vel, accel, hdt, state_size[1], veldot_hist[1], velocity);

      // Advance time to the midpoint.
      step_factor = 0.5;
      break;
   }

   /**
    * - Third intermediate step:@n
    *   Advance state to the end of the integration interval.
    *   It is this step where the CF formalism starts to kick in.
    *   Generalized position is propagated from the position at the
    *   end of step #1 via the exponential map function operating on
    *   (v - posdot_hist[0]/2)*dt. Generalized velocity is propagated
    *   as a simple Euler full step using the input acceleration.@n
    *   The input velocity and acceleration are saved in posdot_hist[2]
    *   and veldot_hist[2] for use on the final step.
    */
   case 3: {

      // Compute the dtheta needed for CF4 step 3 and save the input velocity.
      for (int ii = 0, vel_size = state_size[1]; ii < vel_size; ++ii) {
         dtheta[ii] = (velocity[ii] - 0.5*posdot_hist[0][ii]) * dyn_dt;
         posdot_hist[2][ii] = velocity[ii];
      }

      // Integrate generalized position, starting from the position that
      // resulted after step 1.
      compute_expmap_position_step (saved_pos, dtheta, position);

      // Integrate generalized velocity.
      integ_utils::euler_step_save_deriv (
         init_vel, accel, dyn_dt, state_size[1], veldot_hist[2], velocity);

      step_factor = 1.0;
      break;
   }

   /**
    * - Final step:@n
    *   Advance the initial state (position and velocity) to the end of
    *   the integration interval.
    *   Velocity is advanced using the canonical RK4 step 4 algorithm.
    *   Position is advanced per the CF formalism. This requires two
    *   calls to the expmap function, with the RK4 Butcher tableau 'b' split
    *   into a 2x4 CF 'beta' matrix calculated per the CF formalism.@n
    *   The second row of this matrix is the difference between the RK4 Butcher
    *   tableau 'b' vector and the first row of the beta matrix.
    *   The first row is the solution x to
    *   @f[ \sum_i x_i = 1/2 @f]
    *   @f[ \sum_i x_i c_i = 1/12 @f]
    *   @f[ \sum_i x_i c_i^2 = 0 @f]
    *   @f[ \sum_i x_i \sum_j a_{ij} c_j = 0 @f]
    *   where 'a' and 'c' are from the RK4 Butcher tableau.
    */
   case 4: {
      static const double one_sixth = 1.0/6.0;
      static const double one_twelfth = 1.0/12.0;

      /*
       * rk4cf_beta contains the two parts of the final step of the two stage
       * commutator free equivalent to RK4. See above for details.
       */
      static const double rk4cf_beta[2][4] = {
         {   0.25,      one_sixth, one_sixth, -one_twelfth},
         {-one_twelfth, one_sixth, one_sixth,    0.25}
      };

      // Integrate generalized position.
      // Note that this needs two steps.
      integ_utils::weighted_sum<4> (
         velocity, posdot_hist, rk4cf_beta[0], dyn_dt, state_size[1], dtheta);
      compute_expmap_position_step (init_pos, dtheta, position);

      integ_utils::weighted_sum<4> (
         velocity, posdot_hist, rk4cf_beta[1], dyn_dt, state_size[1], dtheta);
      compute_expmap_position_step (position, dtheta, position);

      // Integrate generalized velocity.
      rk::rk4_final_step (
         init_vel, veldot_hist, accel, dyn_dt, state_size[1],
         velocity);

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
