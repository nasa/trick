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
 * Defines free functions used by a number of integrators.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_INTEG_UTILS_HH
#define ER7_UTILS_INTEG_UTILS_HH

// System includes
#include <cstring>

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"


namespace er7_utils {

/**
 * Miscellaneous integration utilities.
 */
namespace integ_utils {

   /**
    * Copy contents of a state vector.
    * @param[in]  src  Input state vector
    * @param[in]  size State size
    * @param[out] dest Copied state vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   copy_array (
      double const * ER7_UTILS_RESTRICT src,
      int size,
      double * ER7_UTILS_RESTRICT dest)
   {
      std::memcpy (dest, src, size*sizeof(double));
   }


   /**
    * Copy the contents of a pair of state vectors.
    * @param[in]  source_0  Position-sized data to be saved
    * @param[in]  source_1  Velocity-sized data to be saved
    * @param[in]  size      State sizes
    * @param[out] dest_0    Saved position-sized data
    * @param[out] dest_1    Saved velocity-sized data
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   two_state_copy_array (
      double const * ER7_UTILS_RESTRICT source_0,
      double const * ER7_UTILS_RESTRICT source_1,
      int size,
      double * ER7_UTILS_RESTRICT dest_0,
      double * ER7_UTILS_RESTRICT dest_1)
   {
      for (int ii = 0; ii < size; ++ii) {
         dest_0[ii] = source_0[ii];
         dest_1[ii] = source_1[ii];
      }
   }


   /**
    * Copy the contents of a pair of state vectors.
    * @param[in]  source_0  Position-sized data to be saved
    * @param[in]  source_1  Velocity-sized data to be saved
    * @param[in]  size      State sizes
    * @param[out] dest_0    Saved position-sized data
    * @param[out] dest_1    Saved velocity-sized data
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   two_state_copy_array (
      double const * ER7_UTILS_RESTRICT source_0,
      double const * ER7_UTILS_RESTRICT source_1,
      int size[2],
      double * ER7_UTILS_RESTRICT dest_0,
      double * ER7_UTILS_RESTRICT dest_1)
   {
      copy_array (source_0, size[0], dest_0);
      copy_array (source_1, size[1], dest_1);
   }


   /**
    * Scale a vector quantity, in place.
    * @param[in]     scale   Scale factor
    * @param[in]     size    Vector size
    * @param[in,out] vect    Vector, updated in place
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   inplace_scale_array (
      double scale,
      int size,
      double * ER7_UTILS_RESTRICT vect)
   {
      for (int ii = 0; ii < size; ++ii) {
         vect[ii] *= scale;
      }
   }


   /**
    * Scale a vector quantity.
    * @param[in]  src     Input vector
    * @param[in]  scale   Scale factor
    * @param[in]  size    Vector size
    * @param[out] dest    Output vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   scale_array (
      double const * ER7_UTILS_RESTRICT src,
      double scale,
      int size,
      double * ER7_UTILS_RESTRICT dest)
   {
      for (int ii = 0; ii < size; ++ii) {
         dest[ii] = src[ii] * scale;
      }
   }


   /**
    * Make an Euler step of size deltat, with state updated in place.
    * @param[in]     deriv   State derivatives vector
    * @param[in]     deltat  Time step
    * @param[in]     size    State size
    * @param[in,out] state   State vector, updated in place
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   inplace_euler_step (
      double const * ER7_UTILS_RESTRICT deriv,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT state)
   {
      for (int ii = 0; ii < size; ++ii) {
         state[ii] += deriv[ii] * deltat;
      }
   }


   /**
    * Make an Euler step of size deltat, with state updated in place.
    * @param[in]     deriv       State derivatives vector
    * @param[in]     deltat      Time step
    * @param[in]     size        State size
    * @param[out]    init_state  Initial state (copied)
    * @param[in,out] state       State vector, updated in place
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   inplace_euler_step_save_state (
      double const * ER7_UTILS_RESTRICT deriv,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT init_state,
      double * ER7_UTILS_RESTRICT state)
   {
      for (int ii = 0; ii < size; ++ii) {
         init_state[ii] = state[ii];
         state[ii] += deriv[ii] * deltat;
      }
   }


   /**
    * Make an Euler step of size deltat, with state updated in place.
    * @param[in]     deriv       State derivatives vector
    * @param[in]     deltat      Time step
    * @param[in]     size        State size
    * @param[out]    init_deriv  Initial deriv (copied)
    * @param[in,out] state       State vector, updated in place
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   inplace_euler_step_save_deriv (
      double const * ER7_UTILS_RESTRICT deriv,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT init_deriv,
      double * ER7_UTILS_RESTRICT state)
   {
      for (int ii = 0; ii < size; ++ii) {
         init_deriv[ii] = deriv[ii];
         state[ii] += deriv[ii] * deltat;
      }
   }


   /**
    * Make an Euler step of size deltat, with state updated in place.
    * @param[in]     deriv       State derivatives vector
    * @param[in]     deltat      Time step
    * @param[in]     size        State size
    * @param[out]    init_state  Initial state (copied)
    * @param[out]    init_deriv  Initial deriv (copied)
    * @param[in,out] state       State vector, updated in place
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   inplace_euler_step_save_both (
      double const * ER7_UTILS_RESTRICT deriv,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT init_state,
      double * ER7_UTILS_RESTRICT init_deriv,
      double * ER7_UTILS_RESTRICT state)
   {
      for (int ii = 0; ii < size; ++ii) {
         init_state[ii] = state[ii];
         init_deriv[ii] = deriv[ii];
         state[ii] += deriv[ii] * deltat;
      }
   }


   /**
    * Make an Euler step of size deltat.
    * @param[in]  old_state  Input state vector
    * @param[in]  deriv      State derivatives vector
    * @param[in]  deltat     Time step
    * @param[in]  size       State size
    * @param[out] new_state  Updated state vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   euler_step (
      double const * ER7_UTILS_RESTRICT old_state,
      double const * ER7_UTILS_RESTRICT deriv,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT new_state)
   {
      for (int ii = 0; ii < size; ++ii) {
         new_state[ii] = old_state[ii] + deriv[ii] * deltat;
      }
   }


   /**
    * Make an Euler step of size deltat.
    * @param[in]  old_state    Input state vector
    * @param[in]  deriv        State derivatives vector
    * @param[in]  deltat       Time step
    * @param[in]  size         State size
    * @param[out] saved_deriv  Saved derivatives
    * @param[out] new_state    Updated state vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   euler_step_save_deriv (
      double const * ER7_UTILS_RESTRICT old_state,
      double const * ER7_UTILS_RESTRICT deriv,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT saved_deriv,
      double * ER7_UTILS_RESTRICT new_state)
   {
      for (int ii = 0; ii < size; ++ii) {
         saved_deriv[ii] = deriv[ii];
         new_state[ii] = old_state[ii] + deriv[ii] * deltat;
      }
   }


   /**
    * Make an Euler step of size deltat.
    * @param[in]     old_state    Input state vector
    * @param[in]     deriv        State derivatives vector
    * @param[in]     deltat       Time step
    * @param[in]     size         State size
    * @param[out]    saved_state  Copy of pre-update new_state
    * @param[out]    saved_deriv  Saved derivatives
    * @param[in,out] new_state    Updated state vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   euler_step_save_both (
      double const * ER7_UTILS_RESTRICT old_state,
      double const * ER7_UTILS_RESTRICT deriv,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT saved_state,
      double * ER7_UTILS_RESTRICT saved_deriv,
      double * ER7_UTILS_RESTRICT new_state)
   {
      for (int ii = 0; ii < size; ++ii) {
         saved_deriv[ii] = deriv[ii];
         saved_state[ii] = new_state[ii];
         new_state[ii] = old_state[ii] + deriv[ii] * deltat;
      }
   }


   /**
    * Make an in-place Euler step of size deltat.
    * @param[in]     posdot    Position derivatives
    * @param[in]     veldot    Velocity derivatives
    * @param[in]     size      State size
    * @param[in]     deltat    Time step
    * @param[in,out] position  Position
    * @param[in,out] velocity  Velocity
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   inplace_two_state_euler_step (
      double const * ER7_UTILS_RESTRICT posdot,
      double const * ER7_UTILS_RESTRICT veldot,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      for (int ii = 0; ii < size; ++ii) {
         position[ii] += posdot[ii] * deltat;
         velocity[ii] += veldot[ii] * deltat;
      }
   }


   /**
    * Make an in-place Euler step of size deltat.
    * @param[in]     posdot    Position derivatives
    * @param[in]     veldot    Velocity derivatives
    * @param[in]     size      State sizes
    * @param[in]     deltat    Time step
    * @param[in,out] position  Position
    * @param[in,out] velocity  Velocity
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   inplace_two_state_euler_step (
      double const * ER7_UTILS_RESTRICT posdot,
      double const * ER7_UTILS_RESTRICT veldot,
      double deltat,
      int size[2],
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      inplace_euler_step (posdot, deltat, size[0], position);
      inplace_euler_step (veldot, deltat, size[1], velocity);
   }


   /**
    * Second order ODE initial step for the special case of generalized
    * position time derivative being the velocity.
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
   inplace_two_state_euler_step_save_all (
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
    * Second order ODE initial step for the special case of generalized
    * position time derivative being the velocity.
    * @param[out]    posdot       Initial position derivative
    * @param[in]     veldot       Initial velocity derivative
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
   inplace_two_state_euler_step_save_all (
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
      inplace_euler_step_save_both (
         posdot, deltat, size[0], init_pos, init_posdot, position);
      inplace_euler_step_save_both (
         veldot, deltat, size[1], init_vel, init_veldot, velocity);
   }


   /**
    * Second order ODE initial step for the special case of generalized
    * position time derivative being the velocity.
    * @param[in]     accel        Initial acceleration
    * @param[in]     deltat       Time step
    * @param[in]     size         State size
    * @param[out]    init_pos     Initial position
    * @param[out]    init_vel     Initial velocity
    * @param[out]    init_veldot  Initial velocity derivative
    * @param[in,out] position     Updated position vector
    * @param[in,out] velocity     Updated velocity vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   inplace_two_state_euler_step_save_pos_vel_acc (
      double const * ER7_UTILS_RESTRICT accel,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT init_pos,
      double * ER7_UTILS_RESTRICT init_vel,
      double * ER7_UTILS_RESTRICT init_veldot,
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      for (int ii = 0; ii < size; ++ii) {
         init_pos[ii]     = position[ii];
         position[ii]    += velocity[ii]*deltat;
         init_vel[ii]     = velocity[ii];
         init_veldot[ii]  = accel[ii];
         velocity[ii]    += accel[ii]*deltat;
      }
   }


   /**
    * Second order ODE initial step for the special case of generalized
    * position time derivative being the velocity.
    * @param[out]    posdot       Initial position derivative
    * @param[in]     veldot       Initial velocity derivative
    * @param[in]     deltat       Time step
    * @param[in]     size         State size
    * @param[out]    init_pos     Initial position
    * @param[out]    init_vel     Initial velocity
    * @param[out]    init_veldot  Initial velocity derivative
    * @param[in,out] position     Updated position vector
    * @param[in,out] velocity     Updated velocity vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   inplace_two_state_euler_step_save_pos_vel_acc (
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
      inplace_euler_step_save_state (
         posdot, deltat, size[0], init_pos, position);
      inplace_euler_step_save_both (
         veldot, deltat, size[1], init_vel, init_veldot, velocity);
   }


   /**
    * Second order ODE initial step for the special case of generalized
    * position time derivative being the velocity.
    * @param[in]     accel        Initial acceleration
    * @param[in]     deltat       Time step
    * @param[in]     size         State size
    * @param[out]    init_pos     Initial position
    * @param[out]    init_vel     Initial velocity
    * @param[in,out] position     Updated position vector
    * @param[in,out] velocity     Updated velocity vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   inplace_two_state_euler_step_save_pos_vel (
      double const * ER7_UTILS_RESTRICT accel,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT init_pos,
      double * ER7_UTILS_RESTRICT init_vel,
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      for (int ii = 0; ii < size; ++ii) {
         init_pos[ii]     = position[ii];
         position[ii]    += velocity[ii]*deltat;
         init_vel[ii]     = velocity[ii];
         velocity[ii]    += accel[ii]*deltat;
      }
   }


   /**
    * Second order ODE initial step for the special case of generalized
    * position time derivative being the velocity.
    * @param[out]    posdot       Initial position derivative
    * @param[in]     veldot       Initial velocity derivative
    * @param[in]     deltat       Time step
    * @param[in]     size         State size
    * @param[out]    init_pos     Initial position
    * @param[out]    init_vel     Initial velocity
    * @param[in,out] position     Updated position vector
    * @param[in,out] velocity     Updated velocity vector
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   inplace_two_state_euler_step_save_pos_vel (
      double const * ER7_UTILS_RESTRICT posdot,
      double const * ER7_UTILS_RESTRICT veldot,
      double deltat,
      int size[2],
      double * ER7_UTILS_RESTRICT init_pos,
      double * ER7_UTILS_RESTRICT init_vel,
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      inplace_euler_step_save_state (
         posdot, deltat, size[0], init_pos, position);
      inplace_euler_step_save_state (
         veldot, deltat, size[1], init_vel, velocity);
   }


   /**
    * Make an Euler step of size deltat.
    * @param[in]  init_pos  Initial position
    * @param[in]  init_vel  Initial velocity
    * @param[in]  accel    Velocity derivatives
    * @param[in]  size      State size
    * @param[in]  deltat    Time step
    * @param[out] position  Position
    * @param[out] velocity  Velocity
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   two_state_euler_step (
      double const * ER7_UTILS_RESTRICT init_pos,
      double const * ER7_UTILS_RESTRICT init_vel,
      double const * ER7_UTILS_RESTRICT accel,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      for (int ii = 0; ii < size; ++ii) {
         position[ii] = init_pos[ii] + velocity[ii] * deltat;
         velocity[ii] = init_vel[ii] + accel[ii] * deltat;
      }
   }


   /**
    * Make an Euler step of size deltat.
    * @param[in]  init_pos  Initial position
    * @param[in]  init_vel  Initial velocity
    * @param[in]  posdot    Position derivatives
    * @param[in]  veldot    Velocity derivatives
    * @param[in]  deltat    Time step
    * @param[in]  size      State sizes
    * @param[out] position  Position
    * @param[out] velocity  Velocity
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   two_state_euler_step (
      double const * ER7_UTILS_RESTRICT init_pos,
      double const * ER7_UTILS_RESTRICT init_vel,
      double const * ER7_UTILS_RESTRICT posdot,
      double const * ER7_UTILS_RESTRICT veldot,
      double deltat,
      int size[2],
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      euler_step (init_pos, posdot, deltat, size[0], position);
      euler_step (init_vel, veldot, deltat, size[1], velocity);
   }


   /**
    * Make an Euler step of size deltat.
    * @param[in]  init_pos  Initial position
    * @param[in]  init_vel  Initial velocity
    * @param[in]  posdot    Position derivatives
    * @param[in]  veldot    Velocity derivatives
    * @param[in]  size      State size
    * @param[in]  deltat    Time step
    * @param[out] position  Position
    * @param[out] velocity  Velocity
    */
   inline void ER7_UTILS_ALWAYS_INLINE
   two_state_euler_step (
      double const * ER7_UTILS_RESTRICT init_pos,
      double const * ER7_UTILS_RESTRICT init_vel,
      double const * ER7_UTILS_RESTRICT posdot,
      double const * ER7_UTILS_RESTRICT veldot,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      for (int ii = 0; ii < size; ++ii) {
         position[ii] = init_pos[ii] + posdot[ii] * deltat;
         velocity[ii] = init_vel[ii] + veldot[ii] * deltat;
      }
   }


   /**
    * Compute the weighted sum
    * @f[ \Delta x_i = \Delta t \sum_j \dot x_{j,i} w_ji @f]
    * @tparam     nweights    Weights size
    * @param[in]  deriv_hist  State derivatives history
    * @param[in]  weights     Weights on saved derivs and velocity
    * @param[in]  deltat      Time step
    * @param[in]  size        State size
    * @param[out] delta_pos   Product of the weighted sum and deltat
    */
   template <int nweights>
   inline void ER7_UTILS_ALWAYS_INLINE
   weighted_sum (
      double const * ER7_UTILS_RESTRICT const * ER7_UTILS_RESTRICT deriv_hist,
      double const * ER7_UTILS_RESTRICT weights,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT delta_pos)
   {
      for (int ii = 0; ii < size; ++ii) {
         double sum = weights[0] * deriv_hist[0][ii];
         for (int jj = 1; jj < nweights; ++jj) {
            sum += weights[jj] * deriv_hist[jj][ii];
         }
         delta_pos[ii] = sum * deltat;
      }
   }


   /**
    * Compute the weighted sum
    * @f[ \Delta x_i = \Delta t \sum_j \dot x_{j,i} w_ji @f]
    * This differs from the first weighted_sum in that the derivatives
    * history does not contain the most recent derivatives.
    * @tparam     nweights    Weights size
    * @param[in]  deriv       Newest state derivatives
    * @param[in]  deriv_hist  State derivatives history
    * @param[in]  weights     Weights on saved derivs and velocity
    * @param[in]  deltat      Time step
    * @param[in]  size        State size
    * @param[out] delta_pos   Output weighted sums
    */
   template <int nweights>
   inline void ER7_UTILS_ALWAYS_INLINE
   weighted_sum (
      double const * ER7_UTILS_RESTRICT deriv,
      double const * ER7_UTILS_RESTRICT const * ER7_UTILS_RESTRICT deriv_hist,
      double const * ER7_UTILS_RESTRICT weights,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT delta_pos)
   {
      for (int ii = 0; ii < size; ++ii) {
         double sum = weights[nweights-1] * deriv[ii];
         for (int jj = 0; jj < nweights-1; ++jj) {
            sum += weights[jj] * deriv_hist[jj][ii];
         }
         delta_pos[ii] = sum * deltat;
      }
   }


   /**
    * Compute the weighted sum
    * @f[ \Delta x_i = \Delta t \sum_j \dot x_{j,i} w_ji @f]
    * Here the derivatives history once again do not yet contain the most
    * recent derivatives. The history is updated along with computing the sum.
    * @tparam        nweights    Weights size
    * @param[in]     deriv       Newest state derivatives
    * @param[in]     weights     Weights on saved derivs and velocity
    * @param[in]     deltat      Time step
    * @param[in]     size        State size
    * @param[in,out] deriv_hist  State derivatives history
    * @param[out]    delta_pos   Output weighted sums
    */
   template <int nweights>
   inline void ER7_UTILS_ALWAYS_INLINE
   weighted_sum_save_newest (
      double const * ER7_UTILS_RESTRICT deriv,
      double const * ER7_UTILS_RESTRICT weights,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT * ER7_UTILS_RESTRICT deriv_hist,
      double * ER7_UTILS_RESTRICT delta_pos)
   {
      for (int ii = 0; ii < size; ++ii) {
         double sum = weights[nweights-1] * deriv[ii];
         for (int jj = 0; jj < nweights-1; ++jj) {
            sum += weights[jj] * deriv_hist[jj][ii];
         }
         delta_pos[ii] = sum * deltat;
         deriv_hist[nweights-1][ii] = deriv[ii];
      }
   }


   /**
    * Make a weighted step of size deltat.
    * @tparam     nweights    Weights size
    * @param[in]  init_state  Input state vector
    * @param[in]  deriv       Current state derivatives
    * @param[in]  deriv_hist  State derivatives history
    * @param[in]  weights     Weights on saved derivs and velocity
    * @param[in]  deltat      Time step
    * @param[in]  size        State size
    * @param[out] new_state   Output state vector
    */
   template <int nweights>
   inline void ER7_UTILS_ALWAYS_INLINE
   weighted_step (
      double const * ER7_UTILS_RESTRICT init_state,
      double const * ER7_UTILS_RESTRICT deriv,
      double const * ER7_UTILS_RESTRICT const * ER7_UTILS_RESTRICT deriv_hist,
      double const * ER7_UTILS_RESTRICT weights,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT new_state)
   {
      for (int ii = 0; ii < size; ++ii) {
         double weighted_deriv = weights[nweights-1] * deriv[ii];
         for (int jj = 0; jj < nweights-1; ++jj) {
            weighted_deriv += weights[jj] * deriv_hist[jj][ii];
         }
         new_state[ii] = init_state[ii] + weighted_deriv * deltat;
      }
   }


   /**
    * Make a weighted step of size deltat.
    * @tparam     nweights    Weights size
    * @param[in]  init_state  Input state vector
    * @param[in]  deriv_hist  State derivatives history
    * @param[in]  weights     Weights on saved derivs and velocity
    * @param[in]  deltat      Time step
    * @param[in]  size        State size
    * @param[out] new_state   Output state vector
    */
   template <int nweights>
   inline void ER7_UTILS_ALWAYS_INLINE
   weighted_step (
      double const * ER7_UTILS_RESTRICT init_state,
      double const * ER7_UTILS_RESTRICT const * ER7_UTILS_RESTRICT deriv_hist,
      double const * ER7_UTILS_RESTRICT weights,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT new_state)
   {
      for (int ii = 0; ii < size; ++ii) {
         double weighted_deriv = weights[0] * deriv_hist[0][ii];
         for (int jj = 1; jj < nweights; ++jj) {
            weighted_deriv += weights[jj] * deriv_hist[jj][ii];
         }
         new_state[ii] = init_state[ii] + weighted_deriv * deltat;
      }
   }

   /**
    * Make a weighted step of size deltat.
    * @tparam     nweights   Weights size
    * @param[in]     init_state  Input state vector
    * @param[in]     deriv       Current state derivatives
    * @param[in]     weights     Weights on derivs
    * @param[in]     deltat      Time step
    * @param[in]     size        State size
    * @param[in,out] deriv_hist  Saved state derivatives vectors
    * @param[out]    new_state   Updated state vector
    */
   template <int nweights>
   inline void ER7_UTILS_ALWAYS_INLINE
   weighted_step_save_deriv (
      double const * ER7_UTILS_RESTRICT init_state,
      double const * ER7_UTILS_RESTRICT deriv,
      double const * ER7_UTILS_RESTRICT weights,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT * ER7_UTILS_RESTRICT deriv_hist,
      double * ER7_UTILS_RESTRICT new_state)
   {
      for (int ii = 0; ii < size; ++ii) {
         double weighted_deriv = weights[nweights-1] * deriv[ii];
         for (int jj = 0; jj < nweights-1; ++jj) {
            weighted_deriv += weights[jj] * deriv_hist[jj][ii];
         }
         deriv_hist[nweights-1][ii] = deriv[ii];
         new_state[ii] = init_state[ii] + weighted_deriv * deltat;
      }
   }

   /**
    * Make a weighted step of size deltat.
    * @tparam     nweights   Weights size
    * @param[in]     init_state  Input state vector
    * @param[in]     deriv       Current state derivatives
    * @param[in]     weights     Weights on derivs
    * @param[in]     deltat      Time step
    * @param[in]     size        State size
    * @param[out]    state_hist  Saved state vectors
    * @param[in,out] deriv_hist  Saved state derivative vectors
    * @param[in,out] new_state   Updated state vector
    */
   template <int nweights>
   inline void ER7_UTILS_ALWAYS_INLINE
   weighted_step_save_both (
      double const * ER7_UTILS_RESTRICT init_state,
      double const * ER7_UTILS_RESTRICT deriv,
      double const * ER7_UTILS_RESTRICT weights,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT * ER7_UTILS_RESTRICT state_hist,
      double * ER7_UTILS_RESTRICT * ER7_UTILS_RESTRICT deriv_hist,
      double * ER7_UTILS_RESTRICT new_state)
   {
      for (int ii = 0; ii < size; ++ii) {
         double weighted_deriv = weights[nweights-1] * deriv[ii];
         for (int jj = 0; jj < nweights-1; ++jj) {
            weighted_deriv += weights[jj] * deriv_hist[jj][ii];
         }
         state_hist[nweights-1][ii] = new_state[ii];
         deriv_hist[nweights-1][ii] = deriv[ii];
         new_state[ii] = init_state[ii] + weighted_deriv * deltat;
      }
   }


   /**
    * Make a second order ODE Runge Kutta weighted step for a generalized
    * position and generalized velocity.
    * @tparam     nweights     Weights size
    * @param[in]  init_pos     Initial position
    * @param[in]  init_vel     Initial velocity
    * @param[in]  posdot       Current position derivatives
    * @param[in]  veldot       Current velocity derivatives
    * @param[in]  posdot_hist  Position derivatives history
    * @param[in]  veldot_hist  Velocity derivatives history
    * @param[in]  weights      Weights on history, current derivatives
    * @param[in]  deltat       Time step
    * @param[in]  size         State size
    * @param[out] position     Updated position vector
    * @param[out] velocity     Updated velocity vector
    */
   template <int nweights>
   inline void ER7_UTILS_ALWAYS_INLINE
   two_state_weighted_step (
      double const * ER7_UTILS_RESTRICT init_pos,
      double const * ER7_UTILS_RESTRICT init_vel,
      double const * ER7_UTILS_RESTRICT posdot,
      double const * ER7_UTILS_RESTRICT veldot,
      double const * ER7_UTILS_RESTRICT const * ER7_UTILS_RESTRICT posdot_hist,
      double const * ER7_UTILS_RESTRICT const * ER7_UTILS_RESTRICT veldot_hist,
      double const * ER7_UTILS_RESTRICT weights,
      double deltat,
      int size[2],
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      weighted_step<nweights> (
         init_pos, posdot, posdot_hist, weights, deltat, size[0], position);
      weighted_step<nweights> (
         init_vel, veldot, veldot_hist, weights, deltat, size[1], velocity);
   }


   /**
    * Make a second order ODE Runge Kutta weighted step for the special
    * case of velocity being the derivative of position.
    * @tparam     nweights     Weights size
    * @param[in]  init_pos     Initial position
    * @param[in]  init_vel     Initial velocity
    * @param[in]  accel        Current acceleration
    * @param[in]  posdot_hist  Position derivative history
    * @param[in]  veldot_hist  Velocity derivative history
    * @param[in]  weights      Weights on derivative elements
    * @param[in]  deltat       Time step
    * @param[in]  size         State size
    * @param[out] position     Updated position vector
    * @param[out] velocity     Updated velocity vector
    */
   template <int nweights>
   inline void ER7_UTILS_ALWAYS_INLINE
   two_state_weighted_step (
      double const * ER7_UTILS_RESTRICT init_pos,
      double const * ER7_UTILS_RESTRICT init_vel,
      double const * ER7_UTILS_RESTRICT accel,
      double const * ER7_UTILS_RESTRICT const * ER7_UTILS_RESTRICT posdot_hist,
      double const * ER7_UTILS_RESTRICT const * ER7_UTILS_RESTRICT veldot_hist,
      double const * ER7_UTILS_RESTRICT weights,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      for (int ii = 0; ii < size; ++ii) {
         double weighted_pos_deriv = weights[nweights-1]*velocity[ii];
         double weighted_vel_deriv = weights[nweights-1]*accel[ii];
         for (int jj = 0; jj < nweights-1; ++jj) {
            weighted_pos_deriv += weights[jj] * posdot_hist[jj][ii];
            weighted_vel_deriv += weights[jj] * veldot_hist[jj][ii];
         }
         position[ii] = init_pos[ii] + weighted_pos_deriv * deltat;
         velocity[ii] = init_vel[ii] + weighted_vel_deriv * deltat;
      }
   }


   /**
    * Make a second order ODE Runge Kutta weighted step for the special
    * case of velocity being the derivative of position.
    * @tparam        nweights     Weights size
    * @param[in]     init_pos     Initial position
    * @param[in]     init_vel     Initial velocity
    * @param[in]     veldot       Current velocity derivative
    * @param[in]     weights      Weights on derivative elements
    * @param[in]     deltat       Time step
    * @param[in]     size         State size
    * @param[in,out] posdot_hist  Position derivative history
    * @param[in,out] veldot_hist  Velocity derivative history
    * @param[out]    position     Updated position vector
    * @param[out]    velocity     Updated velocity vector
    */
   template <int nweights>
   inline void ER7_UTILS_ALWAYS_INLINE
   two_state_weighted_step_save_veldot (
      double const * ER7_UTILS_RESTRICT init_pos,
      double const * ER7_UTILS_RESTRICT init_vel,
      double const * ER7_UTILS_RESTRICT veldot,
      double const * ER7_UTILS_RESTRICT weights,
      double deltat,
      int size[2],
      double * ER7_UTILS_RESTRICT * ER7_UTILS_RESTRICT posdot_hist,
      double * ER7_UTILS_RESTRICT * ER7_UTILS_RESTRICT veldot_hist,
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      weighted_step<nweights> (
         init_pos, posdot_hist, weights, deltat, size[0], position);
      weighted_step_save_deriv<nweights> (
         init_vel, veldot, weights, deltat, size[1], veldot_hist, velocity);
   }


   /**
    * Make a second order ODE Runge Kutta weighted step for the special
    * case of velocity being the derivative of position.
    * @tparam        nweights     Weights size
    * @param[in]     init_pos     Initial position
    * @param[in]     init_vel     Initial velocity
    * @param[in]     accel        Current acceleration
    * @param[in]     weights      Weights on derivative elements
    * @param[in]     deltat       Time step
    * @param[in]     size         State size
    * @param[in,out] posdot_hist  Position derivative history
    * @param[in,out] veldot_hist  Velocity derivative history
    * @param[out]    position     Updated position vector
    * @param[in,out] velocity     Updated velocity vector
    */
   template <int nweights>
   inline void ER7_UTILS_ALWAYS_INLINE
   two_state_weighted_step_save_derivs (
      double const * ER7_UTILS_RESTRICT init_pos,
      double const * ER7_UTILS_RESTRICT init_vel,
      double const * ER7_UTILS_RESTRICT accel,
      double const * ER7_UTILS_RESTRICT weights,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT * ER7_UTILS_RESTRICT posdot_hist,
      double * ER7_UTILS_RESTRICT * ER7_UTILS_RESTRICT veldot_hist,
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      for (int ii = 0; ii < size; ++ii) {
         double weighted_pos_deriv = weights[nweights-1]*velocity[ii];
         double weighted_vel_deriv = weights[nweights-1]*accel[ii];
         for (int jj = 0; jj < nweights-1; ++jj) {
            weighted_pos_deriv += weights[jj] * posdot_hist[jj][ii];
            weighted_vel_deriv += weights[jj] * veldot_hist[jj][ii];
         }
         posdot_hist[nweights-1][ii] = velocity[ii];
         veldot_hist[nweights-1][ii] = accel[ii];
         position[ii] = init_pos[ii] + weighted_pos_deriv * deltat;
         velocity[ii] = init_vel[ii] + weighted_vel_deriv * deltat;
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
