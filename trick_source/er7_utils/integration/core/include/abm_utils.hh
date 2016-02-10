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
 * Defines free functions used by Adams-Bashforth-Moulton integrators.
 */

/*
Purpose: ()
*/

#ifndef ER7_UTILS_ABM_UTILS_HH
#define ER7_UTILS_ABM_UTILS_HH


// ER7 utilities includes
#include "er7_utils/interface/include/er7_class.hh"


namespace er7_utils {

/**
 * Utility template functions specific to the ABM family of integrators.
 */
namespace abm {

   /**
    * Circularly rotate the history buffer.
    * @tparam        hist_len    Size of the history buffer
    * @param[in,out] deriv_hist  State derivatives history
    */
   template<int hist_len>
   inline void ER7_UTILS_ALWAYS_INLINE
   rotate_history (
      double * ER7_UTILS_RESTRICT deriv_hist[hist_len])
   {
      double * oldest = deriv_hist[hist_len-1];
      for (int jj = hist_len-1; jj > 0; --jj) {
         deriv_hist[jj] = deriv_hist[jj-1];
      }
      deriv_hist[0] = oldest;
   }

   /**
    * Circularly rotate the velocity and acceleration history buffers.
    * @tparam        hist_len    Size of the history buffer
    * @param[in,out] posdot_hist  State derivatives history
    * @param[in,out] veldot_hist  State second derivatives history
    */
   template<int hist_len>
   inline void ER7_UTILS_ALWAYS_INLINE
   rotate_history (
      double * ER7_UTILS_RESTRICT posdot_hist[hist_len],
      double * ER7_UTILS_RESTRICT veldot_hist[hist_len])
   {
      double * oldest_posdot = posdot_hist[hist_len-1];
      double * oldest_veldot = veldot_hist[hist_len-1];
      for (int jj = hist_len-1; jj > 0; --jj) {
         posdot_hist[jj] = posdot_hist[jj-1];
         veldot_hist[jj] = veldot_hist[jj-1];
      }
      posdot_hist[0] = oldest_posdot;
      veldot_hist[0] = oldest_veldot;
   }


   /**
    * Make an Adams-Bashforth predictor step.
    * @tparam        hist_len    Size of the history buffer
    * @param[in]     velocity    State derivatives
    * @param[in]     weights     Weights
    * @param[in]     wscaled_dt  Time step * common weight scale
    * @param[in]     size        State size
    * @param[in,out] deriv_hist  State derivatives history
    * @param[out]    init_pos    Initial state
    * @param[in,out] position    State vector (updated in place)
    */
   template<int hist_len>
   inline void ER7_UTILS_ALWAYS_INLINE
   predictor_step (
      double const * ER7_UTILS_RESTRICT velocity,
      double const * ER7_UTILS_RESTRICT weights,
      double wscaled_dt,
      int size,
      double * ER7_UTILS_RESTRICT deriv_hist[hist_len],
      double * ER7_UTILS_RESTRICT init_pos,
      double * ER7_UTILS_RESTRICT position)
   {
      for (int ii = 0; ii < size; ++ii) {
         // Save the initial position and velocity.
         init_pos[ii]      = position[ii];
         deriv_hist[0][ii] = velocity[ii];

         // Compute the updated position.
         double weighted_deriv = weights[0] * velocity[ii];
         for (int jj = 1; jj < hist_len; ++jj) {
            weighted_deriv += weights[jj] * deriv_hist[jj][ii];
         }
         position[ii] += weighted_deriv * wscaled_dt;
      }
   }


   /**
    * Make an Adams-Bashforth predictor step.
    * @tparam        hist_len    Size of the history buffer
    * @param[in]     position    State vector
    * @param[in]     velocity    State derivatives
    * @param[in]     weights     Weights
    * @param[in]     wscaled_dt  Time step * common weight scale
    * @param[in]     size        State size
    * @param[in,out] deriv_hist  State derivatives history
    * @param[out]    init_pos    Initial state
    * @param[out]    delta_pos   Change in position
    */
   template<int hist_len>
   inline void ER7_UTILS_ALWAYS_INLINE
   predictor_weighted_sum (
      double const * ER7_UTILS_RESTRICT position,
      double const * ER7_UTILS_RESTRICT velocity,
      double const * ER7_UTILS_RESTRICT weights,
      double wscaled_dt,
      int size,
      double * ER7_UTILS_RESTRICT deriv_hist[hist_len],
      double * ER7_UTILS_RESTRICT init_pos,
      double * ER7_UTILS_RESTRICT delta_pos)
   {
      for (int ii = 0; ii < size; ++ii) {
         // Save the initial position and velocity.
         init_pos[ii]      = position[ii];
         deriv_hist[0][ii] = velocity[ii];

         // Compute the change in position.
         double weighted_deriv = weights[0] * velocity[ii];
         for (int jj = 1; jj < hist_len; ++jj) {
            weighted_deriv += weights[jj] * deriv_hist[jj][ii];
         }
         delta_pos[ii] = weighted_deriv * wscaled_dt;
      }
   }


   /**
    * Make an Adams-Bashforth predictor step.
    * @tparam        hist_len    Size of the history buffer
    * @param[in]     velocity    State derivatives
    * @param[in]     weights     Weights
    * @param[in]     wscale      Common weight scale factor
    * @param[in]     deltat      Time step
    * @param[in]     size        State size
    * @param[in,out] deriv_hist  State derivatives history
    * @param[out]    init_pos    Initial state
    * @param[in,out] position    State vector (updated in place)
    */
   template<int hist_len>
   inline void ER7_UTILS_ALWAYS_INLINE
   predictor_step (
      double const * ER7_UTILS_RESTRICT velocity,
      double const * ER7_UTILS_RESTRICT weights,
      double wscale,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT deriv_hist[hist_len],
      double * ER7_UTILS_RESTRICT init_pos,
      double * ER7_UTILS_RESTRICT position)
   {
      predictor_step<hist_len> (
         velocity, weights, wscale*deltat, size,
         deriv_hist, init_pos, position);
   }


   /**
    * Make a second order ODE Adams-Bashforth predictor step for the special
    * case of velocity being the derivative of position.
    * @tparam        hist_len     Size of the history buffer
    * @param[in]     accel        Velocity derivatives
    * @param[in]     weights      Weights
    * @param[in]     wscale       Common weight scale factor
    * @param[in]     deltat       Time step
    * @param[in]     state_size   Size of the position and velocity vectors
    * @param[in,out] posdot_hist  Generalized position derivatives history
    * @param[in,out] veldot_hist  Generalized velocity derivatives history
    * @param[out]    init_pos     Initial position
    * @param[out]    init_vel     Initial velocity
    * @param[in,out] position     Position vector (updated in place)
    * @param[in,out] velocity     Velocity vector (updated in place)
    */
   template<int hist_len>
   inline void ER7_UTILS_ALWAYS_INLINE
   predictor_step (
      double const * ER7_UTILS_RESTRICT accel,
      double const * ER7_UTILS_RESTRICT weights,
      double wscale,
      double deltat,
      int state_size,
      double * ER7_UTILS_RESTRICT posdot_hist[hist_len],
      double * ER7_UTILS_RESTRICT veldot_hist[hist_len],
      double * ER7_UTILS_RESTRICT init_pos,
      double * ER7_UTILS_RESTRICT init_vel,
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      double wscaled_dt = wscale * deltat;
      for (int ii = 0; ii < state_size; ++ii) {
         // Save the initial position, velocity, and their derivatives.
         init_pos[ii]       = position[ii];
         init_vel[ii]       = velocity[ii];
         posdot_hist[0][ii] = velocity[ii];
         veldot_hist[0][ii] = accel[ii];

         // Compute the updated position.
         double weighted_posdot = weights[0] * velocity[ii];
         double weighted_veldot = weights[0] * accel[ii];
         for (int jj = 1; jj < hist_len; ++jj) {
            weighted_posdot += weights[jj] * posdot_hist[jj][ii];
            weighted_veldot += weights[jj] * veldot_hist[jj][ii];
         }
         position[ii] += weighted_posdot * wscaled_dt;
         velocity[ii] += weighted_veldot * wscaled_dt;
      }
   }


   /**
    * Make a second order ODE Adams-Bashforth predictor step for a generalized
    * position and generalized velocity.
    * @tparam        hist_len     Size of the history buffer
    * @param[in]     posdot       Position derivatives
    * @param[in]     veldot       Velocity derivatives
    * @param[in]     weights      Weights
    * @param[in]     wscale       Common weight scale factor
    * @param[in]     deltat       Time step
    * @param[in]     state_size   Sizes of position, velocity vectors
    * @param[in,out] posdot_hist  Generalized position derivatives history
    * @param[in,out] veldot_hist  Generalized velocity derivatives history
    * @param[out]    init_pos     Initial position
    * @param[out]    init_vel     Initial velocity
    * @param[in,out] position     Position vector (updated in place)
    * @param[in,out] velocity     Velocity vector (updated in place)
    */
   template<int hist_len>
   inline void ER7_UTILS_ALWAYS_INLINE
   predictor_step (
      double const * ER7_UTILS_RESTRICT posdot,
      double const * ER7_UTILS_RESTRICT veldot,
      double const * ER7_UTILS_RESTRICT weights,
      double wscale,
      double deltat,
      int state_size[2],
      double * ER7_UTILS_RESTRICT posdot_hist[hist_len],
      double * ER7_UTILS_RESTRICT veldot_hist[hist_len],
      double * ER7_UTILS_RESTRICT init_pos,
      double * ER7_UTILS_RESTRICT init_vel,
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      double wscaled_dt = wscale * deltat;

      predictor_step<hist_len> (
         posdot, weights, wscaled_dt, state_size[0],
         posdot_hist, init_pos, position);

      predictor_step<hist_len> (
         veldot, weights, wscaled_dt, state_size[1],
         veldot_hist, init_vel, velocity);
   }


   /**
    * Make a second order ODE Adams-Bashforth predictor step for a generalized
    * position and generalized velocity, where generalized position will be
    * advanced externally using the delta_pos computed in this function.
    * @tparam        hist_len     Size of the history buffer
    * @param[in]     position     Position
    * @param[in]     posdot       Position derivatives
    * @param[in]     veldot       Velocity derivatives
    * @param[in]     weights      Weights
    * @param[in]     wscale       Common weight scale factor
    * @param[in]     deltat       Time step
    * @param[in]     state_size   Sizes of position, velocity vectors
    * @param[in,out] posdot_hist  Generalized position derivatives history
    * @param[in,out] veldot_hist  Generalized velocity derivatives history
    * @param[out]    init_pos     Initial position
    * @param[out]    init_vel     Initial velocity
    * @param[out]    delta_pos    Position vector delta
    * @param[in,out] velocity     Velocity vector (updated in place)
    */
   template<int hist_len>
   inline void ER7_UTILS_ALWAYS_INLINE
   predictor_step (
      double const * ER7_UTILS_RESTRICT position,
      double const * ER7_UTILS_RESTRICT posdot,
      double const * ER7_UTILS_RESTRICT veldot,
      double const * ER7_UTILS_RESTRICT weights,
      double wscale,
      double deltat,
      int state_size[2],
      double * ER7_UTILS_RESTRICT posdot_hist[hist_len],
      double * ER7_UTILS_RESTRICT veldot_hist[hist_len],
      double * ER7_UTILS_RESTRICT init_pos,
      double * ER7_UTILS_RESTRICT init_vel,
      double * ER7_UTILS_RESTRICT delta_pos,
      double * ER7_UTILS_RESTRICT velocity)
   {
      double wscaled_dt = wscale * deltat;

      predictor_weighted_sum<hist_len> (
         position, posdot, weights, wscaled_dt, state_size[0],
         posdot_hist, init_pos, delta_pos);

      predictor_step<hist_len> (
         veldot, weights, wscaled_dt, state_size[1],
         veldot_hist, init_vel, velocity);
   }


#if 0
   /**
    * Make a second order ODE Adams-Bashforth predictor step for generalized
    * position and velocity, where generalized position is updated afterwards
    * some a position update function and the mean velocity.
    * @tparam        hist_len     Size of the history buffer
    * @param[in]     accel        Velocity derivatives
    * @param[in]     weights      Weights
    * @param[in]     wscale       Common weight scale factor
    * @param[in]     deltat       Time step
    * @param[in]     state_size   Size of the position and velocity vectors
    * @param[in,out] posdot_hist  Generalized position derivatives history
    * @param[in,out] veldot_hist  Generalized velocity derivatives history
    * @param[out]    init_vel     Initial velocity
    * @param[in,out] mean_vel     Mean velocity for position update
    * @param[in,out] velocity     Velocity vector (updated in place)
    */
   template<int hist_len>
   inline void ER7_UTILS_ALWAYS_INLINE
   predictor_step (
      double const * ER7_UTILS_RESTRICT accel,
      double const * ER7_UTILS_RESTRICT weights,
      double wscale,
      double deltat,
      int state_size,
      double * ER7_UTILS_RESTRICT posdot_hist[hist_len],
      double * ER7_UTILS_RESTRICT veldot_hist[hist_len],
      double * ER7_UTILS_RESTRICT init_vel,
      double * ER7_UTILS_RESTRICT mean_vel,
      double * ER7_UTILS_RESTRICT velocity)
   {
      double wscaled_dt = wscale * deltat;
      for (int ii = 0; ii < state_size; ++ii) {
         // Save the initial position, velocity, and their derivatives.
         init_vel[ii]       = velocity[ii];
         posdot_hist[0][ii] = velocity[ii];
         veldot_hist[0][ii] = accel[ii];

         // Compute the mean velocity and update velocity.
         double weighted_posdot = weights[0] * velocity[ii];
         double weighted_veldot = weights[0] * accel[ii];
         for (int jj = 1; jj < hist_len; ++jj) {
            weighted_posdot += weights[jj] * posdot_hist[jj][ii];
            weighted_veldot += weights[jj] * veldot_hist[jj][ii];
         }
         mean_vel[ii]  = weighted_posdot * wscale;
         velocity[ii] += weighted_veldot * wscaled_dt;
      }
   }
#endif


   /**
    * Make an Adams-Moulton corrector step.
    * @tparam        hist_len    Size of the history buffer
    * @param[in]     init_pos    Initial state
    * @param[in]     velocity    State derivatives
    * @param[in]     deriv_hist  State derivatives vectors
    * @param[in]     weights     Weights on velocity, deriv_hist
    * @param[in]     wscaled_dt  Time step * common weight scale
    * @param[in]     size        State size
    * @param[out]    position    Updated state vector
    */
   template<int hist_len>
   inline void ER7_UTILS_ALWAYS_INLINE
   corrector_step (
      double const * ER7_UTILS_RESTRICT init_pos,
      double const * ER7_UTILS_RESTRICT velocity,
      double const * ER7_UTILS_RESTRICT const deriv_hist[hist_len],
      double const * ER7_UTILS_RESTRICT weights,
      double wscaled_dt,
      int size,
      double * ER7_UTILS_RESTRICT position)
   {
      double der_weight = weights[0];
      double const * ER7_UTILS_RESTRICT hist_weights = weights+1;

      for (int ii = 0; ii < size; ++ii) {
         double weighted_deriv = der_weight * velocity[ii];
         for (int jj = 0; jj < hist_len-1; ++jj) {
            weighted_deriv += hist_weights[jj] * deriv_hist[jj][ii];
         }
         position[ii] = init_pos[ii] + weighted_deriv * wscaled_dt;
      }
   }


   /**
    * Make an Adams-Bashforth corrector step.
    * @tparam        hist_len    Size of the history buffer
    * @param[in]     velocity    State derivatives
    * @param[in]     weights     Weights
    * @param[in]     wscaled_dt  Time step * common weight scale
    * @param[in]     size        State size
    * @param[in,out] deriv_hist  State derivatives history
    * @param[out]    delta_pos   Change in position
    */
   template<int hist_len>
   inline void ER7_UTILS_ALWAYS_INLINE
   corrector_weighted_sum (
      double const * ER7_UTILS_RESTRICT velocity,
      double const * ER7_UTILS_RESTRICT const deriv_hist[hist_len],
      double const * ER7_UTILS_RESTRICT weights,
      double wscaled_dt,
      int size,
      double * ER7_UTILS_RESTRICT delta_pos)
   {
      double der_weight = weights[0];
      double const * ER7_UTILS_RESTRICT hist_weights = weights+1;

      for (int ii = 0; ii < size; ++ii) {
         double weighted_deriv = der_weight * velocity[ii];
         for (int jj = 0; jj < hist_len-1; ++jj) {
            weighted_deriv += hist_weights[jj] * deriv_hist[jj][ii];
         }
         delta_pos[ii] = weighted_deriv * wscaled_dt;
      }
   }


   /**
    * Make an Adams-Moulton corrector step.
    * @tparam        hist_len    Size of the history buffer
    * @param[in]     init_pos    Initial state
    * @param[in]     velocity    State derivatives
    * @param[in]     deriv_hist  State derivatives vectors
    * @param[in]     weights     Weights on velocity, deriv_hist
    * @param[in]     wscale      Common weight scale factor
    * @param[in]     deltat      Time step
    * @param[in]     size        State size
    * @param[out]    position    Updated state vector
    */
   template<int hist_len>
   inline void ER7_UTILS_ALWAYS_INLINE
   corrector_step (
      double const * ER7_UTILS_RESTRICT init_pos,
      double const * ER7_UTILS_RESTRICT velocity,
      double const * ER7_UTILS_RESTRICT const deriv_hist[hist_len],
      double const * ER7_UTILS_RESTRICT weights,
      double wscale,
      double deltat,
      int size,
      double * ER7_UTILS_RESTRICT position)
   {
      corrector_step<hist_len> (
         init_pos, velocity, deriv_hist, weights, wscale*deltat, size,
         position);
   }


   /**
    * Make a second order ODE Adams-Moulton corrector step for the special
    * case of velocity being the derivative of position.
    * @tparam        hist_len     Size of the history buffer
    * @param[in]     init_pos     Initial state
    * @param[out]    init_vel     Initial velocity
    * @param[in]     accel        Velocity derivatives
    * @param[in,out] posdot_hist  Generalized position derivatives history
    * @param[in,out] veldot_hist  Generalized velocity derivatives history
    * @param[in]     weights      Weights on velocity, deriv_hist
    * @param[in]     wscale       Common weight scale factor
    * @param[in]     deltat       Time step
    * @param[in]     state_size   State sizes
    * @param[out]    position     Updated position vector
    * @param[out]    velocity     Updated velocity vector
    */
   template<int hist_len>
   inline void ER7_UTILS_ALWAYS_INLINE
   corrector_step (
      double const * ER7_UTILS_RESTRICT init_pos,
      double const * ER7_UTILS_RESTRICT init_vel,
      double const * ER7_UTILS_RESTRICT accel,
      double const * ER7_UTILS_RESTRICT const posdot_hist[hist_len],
      double const * ER7_UTILS_RESTRICT const veldot_hist[hist_len],
      double const * ER7_UTILS_RESTRICT weights,
      double wscale,
      double deltat,
      int state_size,
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      double wscaled_dt = wscale * deltat;
      double der_weight = weights[0];
      double const * ER7_UTILS_RESTRICT hist_weights = weights+1;

      for (int ii = 0; ii < state_size; ++ii) {
         double weighted_posdot = der_weight * velocity[ii];
         double weighted_veldot = der_weight * accel[ii];
         for (int jj = 0; jj < hist_len-1; ++jj) {
            weighted_posdot += hist_weights[jj] * posdot_hist[jj][ii];
            weighted_veldot += hist_weights[jj] * veldot_hist[jj][ii];
         }
         position[ii] = init_pos[ii] + weighted_posdot * wscaled_dt;
         velocity[ii] = init_vel[ii] + weighted_veldot * wscaled_dt;
      }
   }


   /**
    * Make a second order ODE Adams-Moulton corrector step for a generalized
    * position and generalized velocity.
    * @tparam        hist_len     Size of the history buffer
    * @param[in]     init_pos     Initial state
    * @param[in]     init_vel     Initial velocity
    * @param[in]     posdot       Position derivatives
    * @param[in]     veldot       Velocity derivatives
    * @param[in,out] posdot_hist  Generalized position derivatives history
    * @param[in,out] veldot_hist  Generalized velocity derivatives history
    * @param[in]     weights      Weights on velocity, deriv_hist
    * @param[in]     wscale       Common weight scale factor
    * @param[in]     deltat       Time step
    * @param[in]     state_size   State sizes
    * @param[out]    position     Updated position vector
    * @param[out]    velocity     Updated velocity vector
    */
   template<int hist_len>
   inline void ER7_UTILS_ALWAYS_INLINE
   corrector_step (
      double const * ER7_UTILS_RESTRICT init_pos,
      double const * ER7_UTILS_RESTRICT init_vel,
      double const * ER7_UTILS_RESTRICT posdot,
      double const * ER7_UTILS_RESTRICT veldot,
      double const * ER7_UTILS_RESTRICT const posdot_hist[hist_len],
      double const * ER7_UTILS_RESTRICT const veldot_hist[hist_len],
      double const * ER7_UTILS_RESTRICT weights,
      double wscale,
      double deltat,
      int state_size[2],
      double * ER7_UTILS_RESTRICT position,
      double * ER7_UTILS_RESTRICT velocity)
   {
      double wscaled_dt = wscale * deltat;

      corrector_step<hist_len> (
         init_pos, posdot, posdot_hist, weights, wscaled_dt, state_size[0],
         position);

      corrector_step<hist_len> (
         init_vel, veldot, veldot_hist, weights, wscaled_dt, state_size[1],
         velocity);
   }


   /**
    * Make a second order ODE Adams-Moulton corrector step for a generalized
    * position and generalized velocity.
    * @tparam        hist_len     Size of the history buffer
    * @param[in]     init_vel     Initial velocity
    * @param[in]     posdot       Position derivatives
    * @param[in]     veldot       Velocity derivatives
    * @param[in,out] posdot_hist  Generalized position derivatives history
    * @param[in,out] veldot_hist  Generalized velocity derivatives history
    * @param[in]     weights      Weights on velocity, deriv_hist
    * @param[in]     wscale       Common weight scale factor
    * @param[in]     deltat       Time step
    * @param[in]     state_size   State sizes
    * @param[out]    delta_pos    Position vector delta
    * @param[out]    velocity     Updated velocity vector
    */
   template<int hist_len>
   inline void ER7_UTILS_ALWAYS_INLINE
   corrector_step (
      double const * ER7_UTILS_RESTRICT init_vel,
      double const * ER7_UTILS_RESTRICT posdot,
      double const * ER7_UTILS_RESTRICT veldot,
      double const * ER7_UTILS_RESTRICT const posdot_hist[hist_len],
      double const * ER7_UTILS_RESTRICT const veldot_hist[hist_len],
      double const * ER7_UTILS_RESTRICT weights,
      double wscale,
      double deltat,
      int state_size[2],
      double * ER7_UTILS_RESTRICT delta_pos,
      double * ER7_UTILS_RESTRICT velocity)
   {
      double wscaled_dt = wscale * deltat;

      corrector_weighted_sum<hist_len> (
         posdot, posdot_hist, weights, wscaled_dt, state_size[0], delta_pos);

      corrector_step<hist_len> (
         init_vel, veldot, veldot_hist, weights, wscaled_dt, state_size[1],
         velocity);
   }


#if 0
   /**
    * Make a second order ODE Adams-Moulton corrector step for the special
    * case of velocity being the derivative of position.
    * @tparam        hist_len     Size of the history buffer
    * @param[out]    init_vel     Initial velocity
    * @param[in]     accel        Velocity derivatives
    * @param[in,out] posdot_hist  Generalized position derivatives history
    * @param[in,out] veldot_hist  Generalized velocity derivatives history
    * @param[in]     weights      Weights on velocity, deriv_hist
    * @param[in]     wscale       Common weight scale factor
    * @param[in]     deltat       Time step
    * @param[in]     state_size   State sizes
    * @param[in,out] mean_vel     Mean velocity for position update
    * @param[out]    velocity     Updated velocity vector
    */
   template<int hist_len>
   inline void ER7_UTILS_ALWAYS_INLINE
   corrector_step (
      double const * ER7_UTILS_RESTRICT init_vel,
      double const * ER7_UTILS_RESTRICT accel,
      double const * ER7_UTILS_RESTRICT const posdot_hist[hist_len],
      double const * ER7_UTILS_RESTRICT const veldot_hist[hist_len],
      double const * ER7_UTILS_RESTRICT weights,
      double wscale,
      double deltat,
      int state_size,
      double * ER7_UTILS_RESTRICT mean_vel,
      double * ER7_UTILS_RESTRICT velocity)
   {
      double wscaled_dt = wscale * deltat;
      double der_weight = weights[0];
      double const * ER7_UTILS_RESTRICT hist_weights = weights+1;

      for (int ii = 0; ii < state_size; ++ii) {
         double weighted_posdot = der_weight * velocity[ii];
         double weighted_veldot = der_weight * accel[ii];
         for (int jj = 0; jj < hist_len-1; ++jj) {
            weighted_posdot += hist_weights[jj] * posdot_hist[jj][ii];
            weighted_veldot += hist_weights[jj] * veldot_hist[jj][ii];
         }
         mean_vel[ii] = weighted_posdot * wscale;
         velocity[ii] = init_vel[ii] + weighted_veldot * wscaled_dt;
      }
   }
#endif

}
}

#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
