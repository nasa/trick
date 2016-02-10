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
 * Defines free functions used by Runge Kutta integrators.
 */

/*
Purpose: ()
*/

#ifndef ER7_UTILS_RK_UTILS_HH
#define ER7_UTILS_RK_UTILS_HH


// Interface includes
#include "er7_utils/interface/include/er7_class.hh"


namespace er7_utils {

/**
 * Utilities specific to the Runge-Kutta famility of integrators.
 */
namespace rk {

#if 0
   /**
    * Make a weighted step of size deltat.
    * @tparam     nweights   Weights size
    * @param[in]  old_state  Input state vector
    * @param[in]  derivs     State derivatives vectors
    * @param[in]  weights    Weights on deriv1
    * @param[in]  deltat     Time step
    * @param[in]  size       State size
    * @param[out] new_state  Updated state vector
    */
   template <int nweights>
   inline void ER7_UTILS_ALWAYS_INLINE
   weighted_step (
      double const * ER7_UTILS_RESTRICT old_state,
      double const * ER7_UTILS_RESTRICT const * ER7_UTILS_RESTRICT derivs,
      double const * ER7_UTILS_RESTRICT weights,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT new_state)
   {
      for (int ii = 0; ii < size; ++ii) {
         double weighted_deriv = weights[0] * derivs[0][ii];
         for (int jj = 1; jj < nweights; ++jj) {
            weighted_deriv += weights[jj] * derivs[jj][ii];
         }
         new_state[ii] = old_state[ii] + weighted_deriv * deltat;
      }
   }


   /**
    * Make a second order ODE Runge Kutta weighted step for a generalized
    * position and generalized velocity.
    * @tparam     nweights  Weights size
    * @param[in]  init_pos  Initial position
    * @param[in]  init_vel  Initial velocity
    * @param[in]  posdot     Position derivatives
    * @param[in]  veldot     Velocity derivatives
    * @param[in]  weights    Weights on deriv1
    * @param[in]  deltat     Time step
    * @param[in]  size       State size
    * @param[out] position   Updated position vector
    * @param[out] velocity   Updated velocity vector
    */
   template <int nweights>
   inline void ER7_UTILS_ALWAYS_INLINE
   two_state_weighted_step (
      double const * ER7_UTILS_RESTRICT init_pos,
      double const * ER7_UTILS_RESTRICT init_vel,
      double const * ER7_UTILS_RESTRICT const * ER7_UTILS_RESTRICT posdot,
      double const * ER7_UTILS_RESTRICT const * ER7_UTILS_RESTRICT veldot,
      double const * ER7_UTILS_RESTRICT weights,
      double deltat,
      int size[2],
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      weighted_step<nweights> (init_pos, posdot, weights, deltat, size[0],
                               position);
      weighted_step<nweights> (init_vel, veldot, weights, deltat, size[1],
                               velocity);
   }


   /**
    * Make a second order ODE Runge Kutta weighted step for the special
    * case of velocity being the derivative of position.
    * @tparam     nweights  Weights size
    * @param[in]  init_pos  Initial position
    * @param[in]  init_vel  Initial velocity
    * @param[in]  posdot    Position derivatives
    * @param[in]  veldot    Velocity derivatives
    * @param[in]  weights   Weights on derivative elements
    * @param[in]  deltat    Time step
    * @param[in]  size      State size
    * @param[out] position  Updated position vector
    * @param[out] velocity  Updated velocity vector
    */
   template <int nweights>
   inline void ER7_UTILS_ALWAYS_INLINE
   two_state_weighted_step (
      double const * ER7_UTILS_RESTRICT init_pos,
      double const * ER7_UTILS_RESTRICT init_vel,
      double const * ER7_UTILS_RESTRICT const * ER7_UTILS_RESTRICT posdot,
      double const * ER7_UTILS_RESTRICT const * ER7_UTILS_RESTRICT veldot,
      double const * ER7_UTILS_RESTRICT weights,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
#if 1
      double weight_j = weights[0];
      double const * ER7_UTILS_RESTRICT posdot_j = posdot[0];
      double const * ER7_UTILS_RESTRICT veldot_j = veldot[0];
      for (int ii = 0; ii < size; ++ii) {
         position[ii] = weight_j * posdot_j[ii];
         velocity[ii] = weight_j * veldot_j[ii];
      }
      for (int jj = 1; jj < nweights; ++jj) {
         weight_j = weights[jj];
         posdot_j = posdot[jj];
         veldot_j = veldot[jj];
         for (int ii = 0; ii < size; ++ii) {
            position[ii] += weight_j * posdot_j[ii];
            velocity[ii] += weight_j * veldot_j[ii];
         }
      }

      for (int ii = 0; ii < size; ++ii) {
         position[ii] *= deltat;
         velocity[ii] *= deltat;
         position[ii] += init_pos[ii];
         velocity[ii] += init_vel[ii];
      }
#else
      for (int ii = 0; ii < size; ++ii) {
         double weighted_pos_deriv = 0.0;
         double weighted_vel_deriv = 0.0;
         for (int jj = 0; jj < nweights; ++jj) {
            weighted_pos_deriv += weights[jj] * posdot[jj][ii];
            weighted_vel_deriv += weights[jj] * veldot[jj][ii];
         }
         position[ii] = old_state[0][ii] + weighted_pos_deriv * deltat;
         velocity[ii] = old_state[1][ii] + weighted_vel_deriv * deltat;
      }
#endif
   }
#endif


   /**
    * Special case for an initial RK step.@n
    * All versions update the state in-place.
    * This version also saves the initial state and initial derivative.
    * @param[in]     deriv       Input state derivative
    * @param[in]     deltat      Time step
    * @param[in]     size        State size
    * @param[out]    init_state  Initial state vector
    * @param[out]    init_deriv  Initial deriv vector
    * @param[in,out] state       Input and updated state vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   rk_initial_step (
      double const * ER7_UTILS_RESTRICT deriv,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT init_state,
      double * ER7_UTILS_RESTRICT init_deriv,
      double * ER7_UTILS_RESTRICT state)
   {
#if 0
#else
      double work_state, work_deriv;
#endif
      for (int ii = 0; ii < size; ++ii) {
#if 0
         init_state[ii] = state[ii];
         init_deriv[ii] = deriv[ii];
         state[ii] += deriv[ii]*deltat;
#else
         work_state = state[ii];
         work_deriv = deriv[ii];
         init_state[ii] = work_state;
         init_deriv[ii] = work_deriv;
         state[ii]      = work_state + work_deriv*deltat;
#endif
      }
   }


   /**
    * Special case for an initial RK step.@n
    * All versions update the state in-place.
    * This version saves the initial state but not the initial derivative.
    * @param[in]     deriv       Input state derivative
    * @param[in]     deltat      Time step
    * @param[in]     size        State size
    * @param[out]    init_state  Initial state vector
    * @param[in,out] state       Input and updated state vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   rk_initial_step (
      double const * ER7_UTILS_RESTRICT deriv,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT init_state,
      double * ER7_UTILS_RESTRICT state)
   {
      for (int ii = 0; ii < size; ++ii) {
         init_state[ii] = state[ii];
         state[ii] += deriv[ii]*deltat;
      }
   }


   /**
    * Second order ODE initial RK step for the general case of generalized
    * position time derivative passed as an argument.@n
    * All versions update the position and velocity in-place.
    * This version also saves the initial position, initial velocity,
    * initial velocity derivative, and initial position derivative.
    * @param[in]     posdot       Initial position time derivative
    * @param[in]     veldot       Initial velocity time derivative
    * @param[in]     deltat       Time step
    * @param[in]     size         State sizes
    * @param[out]    init_pos     Initial position
    * @param[out]    init_vel     Initial velocity
    * @param[out]    init_posdot  Initial position time derivative
    * @param[out]    init_veldot  Initial velocity time derivative
    * @param[in,out] position     Updated position vector
    * @param[in,out] velocity     Updated velocity vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   rk_two_state_initial_step (
      double const * ER7_UTILS_RESTRICT posdot,
      double const * ER7_UTILS_RESTRICT veldot,
      double deltat,
      int size[2],
      double * ER7_UTILS_RESTRICT init_pos,
      double * ER7_UTILS_RESTRICT init_vel,
      double * ER7_UTILS_RESTRICT init_posdot,
      double * ER7_UTILS_RESTRICT init_veldot,
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      rk_initial_step (posdot, deltat, size[0], init_pos,
                       init_posdot, position);
      rk_initial_step (veldot, deltat, size[1], init_vel,
                       init_veldot, velocity);
   }


   /**
    * Second order ODE initial RK step for the general case of generalized
    * position time derivative passed as an argument.@n
    * All versions update the position and velocity in-place.
    * This version also saves the initial position, initial velocity, and
    * initial velocity derivative, but not the initial position derivative.
    * @param[in]     posdot       Initial position time derivative
    * @param[in]     veldot       Initial velocity time derivative
    * @param[in]     deltat       Time step
    * @param[in]     size         State sizes
    * @param[out]    init_pos     Initial position
    * @param[out]    init_vel     Initial velocity
    * @param[out]    init_veldot  Initial velocity time derivative
    * @param[in,out] position     Updated position vector
    * @param[in,out] velocity     Updated velocity vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   rk_two_state_initial_step (
      double const * ER7_UTILS_RESTRICT posdot,
      double const * ER7_UTILS_RESTRICT veldot,
      double deltat,
      int size[2],
      double * ER7_UTILS_RESTRICT init_pos,
      double * ER7_UTILS_RESTRICT init_vel,
      double * ER7_UTILS_RESTRICT init_veldot,
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      rk_initial_step (posdot, deltat, size[0],
                       init_pos, position);
      rk_initial_step (veldot, deltat, size[1],
                       init_vel, init_veldot, velocity);
   }


   /**
    * Second order ODE initial RK step for the special case of generalized
    * position time derivative being the velocity.@n
    * All versions update the position and velocity in-place.
    * This version also saves the initial position, velocity, and acceleration,
    * an also explicitly saves the initial position derivative.
    * @param[in]     accel        Initial acceleration
    * @param[in]     deltat       Time step
    * @param[in]     size         State size
    * @param[out]    init_pos     Initial position
    * @param[out]    init_vel     Initial velocity
    * @param[out]    init_posdot  Initial position derivative
    * @param[out]    init_veldot  Initial velocity derivative
    * @param[in,out] position     Updated position vector
    * @param[in,out] velocity     Updated velocity vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   rk_two_state_initial_step (
      double const * ER7_UTILS_RESTRICT accel,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT init_pos,
      double * ER7_UTILS_RESTRICT init_vel,
      double * ER7_UTILS_RESTRICT init_posdot,
      double * ER7_UTILS_RESTRICT init_veldot,
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      for (int ii = 0; ii < size; ++ii) {
         init_pos[ii]     = position[ii];
         init_posdot[ii]  = velocity[ii];
         position[ii]    += velocity[ii]*deltat;
         init_vel[ii]     = velocity[ii];
         init_veldot[ii]  = accel[ii];
         velocity[ii]    += accel[ii]*deltat;
      }
   }


   /**
    * Second order ODE initial RK step for the special case of generalized
    * position time derivative being the velocity.@n
    * All versions update the position and velocity in-place.
    * This version also saves the initial position, velocity, and acceleration,
    * but does not explicitly save the initial position derivative.
    * @param[in]     accel     Initial acceleration
    * @param[in]     deltat    Time step
    * @param[in]     size      State size
    * @param[out]    init_pos  Initial position
    * @param[out]    init_vel  Initial velocity
    * @param[out]    init_acc  Initial acceleration
    * @param[in,out] position  Updated position vector
    * @param[in,out] velocity  Updated velocity vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   rk_two_state_initial_step (
      double const * ER7_UTILS_RESTRICT accel,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT init_pos,
      double * ER7_UTILS_RESTRICT init_vel,
      double * ER7_UTILS_RESTRICT init_acc,
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      for (int ii = 0; ii < size; ++ii) {
         init_pos[ii]  = position[ii];
         position[ii] += velocity[ii]*deltat;
         init_vel[ii]  = velocity[ii];
         init_acc[ii]  = accel[ii];
         velocity[ii] += accel[ii]*deltat;
      }
   }


   /**
    * Special case for an intermediate RK step.
    * @param[in]     init_state   Initial state vector
    * @param[in]     curr_deriv   Input state derivative
    * @param[in]     deltat       Time step
    * @param[in]     size         State size
    * @param[out]    saved_deriv  Saved deriv vector
    * @param[in,out] state        Updated state vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   rk_intermediate_step (
      double const * ER7_UTILS_RESTRICT init_state,
      double const * ER7_UTILS_RESTRICT curr_deriv,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT saved_deriv,
      double * ER7_UTILS_RESTRICT state)
   {
      double work_state, work_deriv;
      for (int ii = 0; ii < size; ++ii) {
         work_state = init_state[ii];
         work_deriv = curr_deriv[ii];
         state[ii]  = work_state + work_deriv * deltat;
         saved_deriv[ii] = work_deriv;
      }
   }


   /**
    * Second order ODE intermediate RK step for the general case of generalized
    * position time derivative passed as an argument.
    * @param[in]     init_pos      Initial position
    * @param[in]     init_vel      Initial velocity
    * @param[in]     posdot        Current position time derivative
    * @param[in]     veldot        Current velocity time derivative
    * @param[in]     deltat        Time step
    * @param[in]     size          State sizes
    * @param[out]    saved_veldot  Updated velocity derivative history
    * @param[in,out] position      Updated position vector
    * @param[in,out] velocity      Updated velocity vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   rk_two_state_intermediate_step (
      double const * ER7_UTILS_RESTRICT init_pos,
      double const * ER7_UTILS_RESTRICT init_vel,
      double const * ER7_UTILS_RESTRICT posdot,
      double const * ER7_UTILS_RESTRICT veldot,
      double deltat,
      int size[2],
      double * ER7_UTILS_RESTRICT saved_veldot,
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      int lim = size[0];
      for (int ii = 0; ii < lim; ++ii) {
         position[ii]     = init_pos[ii] + posdot[ii]*deltat;
      }

      lim = size[1];
      for (int ii = 0; ii < lim; ++ii) {
         saved_veldot[ii] = veldot[ii];
         velocity[ii]     = init_vel[ii] + veldot[ii]*deltat;
      }
   }


   /**
    * Second order ODE intermediate RK step for the special case of generalized
    * position time derivative being the velocity.
    * @param[in]     init_pos   Initial position
    * @param[in]     init_vel   Initial velocity
    * @param[in]     accel      Current acceleration
    * @param[in]     deltat     Time step
    * @param[in]     size       State size
    * @param[out]    saved_vel  Updated velocity history
    * @param[out]    saved_acc  Updated acceleration history
    * @param[in,out] position   Updated position vector
    * @param[in,out] velocity   Updated velocity vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   rk_two_state_intermediate_step (
      double const * ER7_UTILS_RESTRICT init_pos,
      double const * ER7_UTILS_RESTRICT init_vel,
      double const * ER7_UTILS_RESTRICT accel,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT saved_vel,
      double * ER7_UTILS_RESTRICT saved_acc,
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      for (int ii = 0; ii < size; ++ii) {
         saved_vel[ii] = velocity[ii];
         saved_acc[ii] = accel[ii];
         position[ii] = init_pos[ii] + velocity[ii]*deltat;
         velocity[ii] = init_vel[ii] + accel[ii]*deltat;
      }
   }


   /**
    * Special case for Heun's method target stage = 2.
    * @param[in]  init_state  Initial state vector
    * @param[in]  deriv1      State derivatives vector
    * @param[in]  deriv2      State derivatives vector
    * @param[in]  deltat      Time step
    * @param[in]  size        State size
    * @param[out] state       Updated state vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   rk2_heun_final_step (
      double const * ER7_UTILS_RESTRICT init_state,
      double const * ER7_UTILS_RESTRICT deriv1,
      double const * ER7_UTILS_RESTRICT deriv2,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT state)
   {
      double hdt = 0.5 * deltat;
      for (int ii = 0; ii < size; ++ii) {
         state[ii] = init_state[ii] + (deriv1[ii] + deriv2[ii]) * hdt;
      }
   }

#if 0
   /**
    * Second order ODE Heun's method final step for the general
    * case of generalized position time derivative passed as an argument.
    * @param[in]     init_pos     Initial position
    * @param[in]     init_vel     Initial velocity
    * @param[in]     init_posdot  Initial position time derivative
    * @param[in]     init_veldot  Initial velocity time derivative
    * @param[in]     curr_posdot  Current position time derivative
    * @param[in]     curr_veldot  Current velocity time derivative
    * @param[in]     deltat       Time step
    * @param[in]     size         State sizes
    * @param[in,out] position     Updated position vector
    * @param[in,out] velocity     Updated velocity vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   rk2_heun_two_state_final_step (
      double const * ER7_UTILS_RESTRICT init_pos,
      double const * ER7_UTILS_RESTRICT init_vel,
      double const * ER7_UTILS_RESTRICT init_posdot,
      double const * ER7_UTILS_RESTRICT init_veldot,
      double const * ER7_UTILS_RESTRICT curr_posdot,
      double const * ER7_UTILS_RESTRICT curr_veldot,
      double deltat,
      int size[2],
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      double hdt = 0.5 * deltat;

      int lim = size[0];
      for (int ii = 0; ii < lim; ++ii) {
         position[ii] = init_pos[ii] +
                        (init_posdot[ii] + curr_posdot[ii]) * hdt;
      }

      lim = size[1];
      for (int ii = 0; ii < lim; ++ii) {
         velocity[ii] = init_vel[ii] +
                        (init_veldot[ii] + curr_veldot[ii]) * hdt;
      }
   }


   /**
    * Second order ODE Heun's method final step for the special
    * case of velocity being the derivative of position.
    * @param[in]     init_pos  Initial position
    * @param[in]     init_vel  Initial velocity
    * @param[in]     init_acc  Initial acceleration
    * @param[in]     accel     Current acceleration
    * @param[in]     deltat    Time step
    * @param[in]     size      State size
    * @param[out]    position  Updated position vector
    * @param[out]    velocity  Updated velocity vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   rk2_heun_two_state_final_step (
      double const * ER7_UTILS_RESTRICT init_pos,
      double const * ER7_UTILS_RESTRICT init_vel,
      double const * ER7_UTILS_RESTRICT init_acc,
      double const * ER7_UTILS_RESTRICT accel,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      double hdt = 0.5 * deltat;
      for (int ii = 0; ii < size; ++ii) {
         position[ii] = init_pos[ii] + (init_vel[ii] + velocity[ii]) * hdt;
         velocity[ii] = init_vel[ii] + (init_acc[ii] + accel[ii]) * hdt;
      }
   }
#endif


   /**
    * Special case for canonical RK4 target stage = 4.
    * @param[in]  init_state  Initial state vector
    * @param[in]  deriv_hist  Intermediate step state derivatives vectors
    * @param[in]  curr_deriv  Current state derivative
    * @param[in]  deltat      Time step
    * @param[in]  size        State size
    * @param[out] state       Updated state vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   rk4_final_step (
      double const * ER7_UTILS_RESTRICT init_state,
      double const * ER7_UTILS_RESTRICT const * ER7_UTILS_RESTRICT deriv_hist,
      double const * ER7_UTILS_RESTRICT curr_deriv,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT state)
   {
      static const double one_sixth = 1.0 / 6.0;
      double dto6 = deltat * one_sixth;
#if 0
      double state_init, deriv0_i, deriv1_i, deriv2_i, deriv3_i;
#endif
#if 1
      const double * ER7_UTILS_RESTRICT deriv0 = deriv_hist[0];
      const double * ER7_UTILS_RESTRICT deriv1 = deriv_hist[1];
      const double * ER7_UTILS_RESTRICT deriv2 = deriv_hist[2];
#endif
      for (int ii = 0; ii < size; ++ii) {
#if 0
         state_init = init_state[ii];
         deriv0_i = deriv_hist[0][ii];
         deriv1_i = deriv_hist[1][ii];
         deriv2_i = deriv_hist[2][ii];
         deriv3_i = curr_deriv[ii];
         state[ii] = init_state[ii] +
                     (deriv0_i + 2.0*(deriv1_i + deriv2_i) + deriv3_i) * dto6;
#endif
#if 1
         state[ii] = init_state[ii] +
                     (deriv0[ii] +
                      2.0*(deriv1[ii] + deriv2[ii]) +
                      curr_deriv[ii]) * dto6;
#else
         state[ii] = init_state[ii] +
                     (deriv_hist[0][ii] +
                      2.0*(deriv_hist[1][ii] + deriv_hist[2][ii]) +
                      curr_deriv[ii]) * dto6;
#endif
      }
   }


   /**
    * Second order ODE final RK step for the special case of generalized
    * position time derivative being the velocity.
    * @param[in]     init_pos   Initial position
    * @param[in]     init_vel   Initial velocity
    * @param[out]    saved_vel  Velocity history
    * @param[out]    saved_acc  Acceleration history
    * @param[in]     posdot     Current position time derivative
    * @param[in]     veldot     Current velocity time derivative
    * @param[in]     deltat        Time step
    * @param[in]     size       State sizes
    * @param[in,out] position   Updated position vector
    * @param[in,out] velocity   Updated velocity vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   rk4_two_state_final_step (
      double const * ER7_UTILS_RESTRICT init_pos,
      double const * ER7_UTILS_RESTRICT init_vel,
      double const * ER7_UTILS_RESTRICT const * ER7_UTILS_RESTRICT saved_vel,
      double const * ER7_UTILS_RESTRICT const * ER7_UTILS_RESTRICT saved_acc,
      double const * ER7_UTILS_RESTRICT posdot,
      double const * ER7_UTILS_RESTRICT veldot,
      double deltat,
      int size[2],
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      static const double one_sixth = 1.0 / 6.0;
      double dto6 = deltat * one_sixth;
      int lim = size[0];
      for (int ii = 0; ii < lim; ++ii) {
         position[ii] = init_pos[ii] +
                        (saved_vel[0][ii] +
                         2.0*(saved_vel[1][ii] + saved_vel[2][ii]) +
                         posdot[ii]) * dto6;
      }

      lim = size[1];
      for (int ii = 0; ii < lim; ++ii) {
         velocity[ii] = init_vel[ii] +
                        (saved_acc[0][ii] +
                         2.0*(saved_acc[1][ii] + saved_acc[2][ii]) +
                         veldot[ii]) * dto6;
      }
   }


   /**
    * Second order ODE final RK step for the special case of generalized
    * position time derivative being the velocity.
    * @param[in]     init_pos   Initial position
    * @param[in]     init_vel   Initial velocity
    * @param[out]    saved_vel  Velocity history
    * @param[out]    saved_acc  Acceleration history
    * @param[in]     accel      Current acceleration
    * @param[in]     deltat     Time step
    * @param[in]     size       State size
    * @param[in,out] position   Updated position vector
    * @param[in,out] velocity   Updated velocity vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   rk4_two_state_final_step (
      double const * ER7_UTILS_RESTRICT init_pos,
      double const * ER7_UTILS_RESTRICT init_vel,
      double const * ER7_UTILS_RESTRICT const * ER7_UTILS_RESTRICT saved_vel,
      double const * ER7_UTILS_RESTRICT const * ER7_UTILS_RESTRICT saved_acc,
      double const * ER7_UTILS_RESTRICT accel,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      static const double one_sixth = 1.0 / 6.0;
      double dto6 = deltat * one_sixth;
      for (int ii = 0; ii < size; ++ii) {
         position[ii] = init_pos[ii] +
                        (saved_vel[0][ii] +
                         2.0*(saved_vel[1][ii] + saved_vel[2][ii]) +
                         velocity[ii]) * dto6;
         velocity[ii] = init_vel[ii] +
                        (saved_acc[0][ii] +
                         2.0*(saved_acc[1][ii] + saved_acc[2][ii]) +
                         accel[ii]) * dto6;
      }
   }

}
}

#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
