/*******************************************************************************

Purpose:
  (Define member functions for the class RKNystrom4SecondOrderODEIntegrator.)

Library dependencies:
  ((rkn4_second_order_ode_integrator.o)
   (er7_utils/integration/core/second_order_ode_integrator.o)
   (er7_utils/interface/alloc.o))

Programmers:
  (((David Hammen) (OSR) (March 2009) (JEOD_2.0 #22) (Initial version))
   ((David Hammen) (OSR) (Sept 2011) (JEOD 2.2 #537)
    (Multi-cycle support)))

*******************************************************************************/

// System includes
#include <cstddef>

// Interface includes
#include "er7_utils/interface/include/alloc.hh"

// Integration includes
#include "er7_utils/integration/core/include/integ_utils.hh"
#include "er7_utils/integration/core/include/rk_utils.hh"

// Model includes
#include "../include/rkn4_second_order_ode_integrator.hh"


namespace er7_utils {

// Default constructor for an RKNystrom4SecondOrderODEIntegrator.
RKNystrom4SecondOrderODEIntegrator::RKNystrom4SecondOrderODEIntegrator (
   void)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (),
   init_pos (NULL),
   init_vel (NULL),
   mean_vel (NULL),
   dtheta (NULL)
{
   // Initialize memory used by fourth order Runge Kutta Nystrom.
   alloc::initialize_2D_array<4> (veldot_hist);
}


// Copy constructor.
RKNystrom4SecondOrderODEIntegrator::RKNystrom4SecondOrderODEIntegrator (
   const RKNystrom4SecondOrderODEIntegrator & src)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (src),
   init_pos (NULL),
   init_vel (NULL),
   mean_vel (NULL),
   dtheta (NULL)
{
   // Replicate the source's contents if they exist.
   if (problem_type == Integration::GeneralizedStepSecondOrderODE) {
      init_pos  = alloc::replicate_array (state_size[0], src.init_pos);
      init_vel  = alloc::replicate_array (state_size[1], src.init_vel);
      dtheta    = alloc::replicate_array (state_size[1], src.dtheta);
      alloc::replicate_2D_array<4> (state_size[1], src.veldot_hist,
                                    veldot_hist);
   }

   else if (src.init_pos != NULL) {
      init_pos = alloc::replicate_array (state_size[0], src.init_pos);
      init_vel = alloc::replicate_array (state_size[1], src.init_vel);
      mean_vel = alloc::replicate_array (state_size[1], src.dtheta);
      alloc::replicate_2D_array<4> (state_size[1], src.veldot_hist,
                                    veldot_hist);
   }

   else {
      alloc::initialize_2D_array<4> (veldot_hist);
   }
}


// Non-default constructor for an RKNystrom4SecondOrderODEIntegrator
// in which the time derivative of position is the generalized velocity.
RKNystrom4SecondOrderODEIntegrator::RKNystrom4SecondOrderODEIntegrator (
   unsigned int size,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (size, controls),
   init_pos (NULL),
   init_vel (NULL),
   mean_vel (NULL),
   dtheta (NULL)
{
   // Allocate memory used by fourth order Runge Kutta Nystrom.
   init_pos = alloc::allocate_array (size);
   init_vel = alloc::allocate_array (size);
   mean_vel = alloc::allocate_array (size);
   alloc::allocate_2D_array<4> (size, veldot_hist);
}


// Non-default constructor for an RKNystrom4SecondOrderODEIntegrator
// for generalized position, generalized velocity.
RKNystrom4SecondOrderODEIntegrator::RKNystrom4SecondOrderODEIntegrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionStepFunctions & step_funs,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (position_size, velocity_size, step_funs, controls),
   init_pos (NULL),
   init_vel (NULL),
   mean_vel (NULL),
   dtheta (NULL)
{
   // Allocate memory used by Runge Kutta 4 algorithm.
   init_pos = alloc::allocate_array (position_size);
   init_vel = alloc::allocate_array (velocity_size);
   dtheta   = alloc::allocate_array (velocity_size);
   alloc::allocate_2D_array<4> (velocity_size, veldot_hist);
}


// RKNystrom4SecondOrderODEIntegrator destructor.
RKNystrom4SecondOrderODEIntegrator::~RKNystrom4SecondOrderODEIntegrator (
   void)
{
   alloc::deallocate_array (init_pos);
   alloc::deallocate_array (init_vel);
   alloc::deallocate_array (mean_vel);
   alloc::deallocate_2D_array<4> (veldot_hist);
}


// Non-throwing swap.
void
RKNystrom4SecondOrderODEIntegrator::swap (
   RKNystrom4SecondOrderODEIntegrator & other)
{
   SecondOrderODEIntegrator::swap (other);

   std::swap (init_pos, other.init_pos);
   std::swap (init_vel, other.init_vel);
   std::swap (mean_vel, other.mean_vel);
   std::swap (dtheta, other.dtheta);
   for (int ii = 0; ii < 4; ++ii) {
      std::swap (veldot_hist[ii], other.veldot_hist[ii]);
   }
}


// Clone a RKNystrom4SimpleSecondOrderODEIntegrator.
RKNystrom4SimpleSecondOrderODEIntegrator *
RKNystrom4SimpleSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Clone a RKNystrom4GeneralizedStepSecondOrderODEIntegrator.
RKNystrom4GeneralizedStepSecondOrderODEIntegrator *
RKNystrom4GeneralizedStepSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


/**
 * Simple second order ODE fourth order Runge Kutta Nystrom step 1.
 * @param[in]     accel        Initial acceleration
 * @param[in]     hdt          Half of the integration time step
 * @param[in]     size         State size
 * @param[out]    init_pos     Initial position
 * @param[out]    init_vel     Initial velocity
 * @param[out]    init_veldot  Initial velocity derivative
 * @param[in,out] position     Updated position vector
 * @param[in,out] velocity     Updated velocity vector
 */
inline void ER7_UTILS_ALWAYS_INLINE
simple_rkn4_step_one (
   double const * ER7_UTILS_RESTRICT accel,
   double hdt,
   int size,
   double * ER7_UTILS_RESTRICT init_pos,
   double * ER7_UTILS_RESTRICT init_vel,
   double * ER7_UTILS_RESTRICT init_veldot,
   double * ER7_UTILS_RESTRICT position,
   double * ER7_UTILS_RESTRICT velocity)
{
   for (int ii = 0; ii < size; ++ii) {
      double adt = accel[ii]*hdt;
      double mean_vel = velocity[ii] + 0.5*adt;
      init_pos[ii]     = position[ii];
      position[ii]    += mean_vel*hdt;
      init_vel[ii]     = velocity[ii];
      init_veldot[ii]  = accel[ii];
      velocity[ii]    += adt;
   }
}


/**
 * Simple second order ODE fourth order Runge Kutta Nystrom step 1.
 * @param[in]     init_pos     Initial position
 * @param[in]     init_vel     Initial velocity
 * @param[in]     accel        Initial acceleration
 * @param[in]     delta_t      Integration time step
 * @param[in]     size         State size
 * @param[out]    veldot       Saved velocity derivative
 * @param[in,out] position     Updated position vector
 * @param[in,out] velocity     Updated velocity vector
 */
inline void ER7_UTILS_ALWAYS_INLINE
simple_rkn4_step_three (
   double const * ER7_UTILS_RESTRICT init_pos,
   double const * ER7_UTILS_RESTRICT init_vel,
   double const * ER7_UTILS_RESTRICT accel,
   double delta_t,
   int size,
   double * ER7_UTILS_RESTRICT veldot,
   double * ER7_UTILS_RESTRICT position,
   double * ER7_UTILS_RESTRICT velocity)
{
   for (int ii = 0; ii < size; ++ii) {
      double adt = accel[ii]*delta_t;
      double mean_vel = velocity[ii] + 0.5*adt;
      position[ii] = init_pos[ii] + mean_vel * delta_t;
      veldot[ii]   = accel[ii];
      velocity[ii] = init_vel[ii] + adt;
   }
}


/**
 * Simple second order ODE fourth order Runge Kutta Nystrom step four.
 * @param[in]     init_pos     Initial position
 * @param[in]     init_vel     Initial velocity
 * @param[in]     accel        Final acceleration
 * @param[in]     veldot_hist  Velocity derivatives history
 * @param[in]     delta_t      Integration time step
 * @param[in]     size         State size
 * @param[in,out] position     Updated position vector
 * @param[in,out] velocity     Updated velocity vector
 */
inline void ER7_UTILS_ALWAYS_INLINE
simple_rkn4_step_four (
   double const * ER7_UTILS_RESTRICT init_pos,
   double const * ER7_UTILS_RESTRICT init_vel,
   double const * ER7_UTILS_RESTRICT accel,
   double const * ER7_UTILS_RESTRICT const * ER7_UTILS_RESTRICT veldot_hist,
   double delta_t,
   int size,
   double * ER7_UTILS_RESTRICT position,
   double * ER7_UTILS_RESTRICT velocity)
{
   static const double one_sixth = 1.0 / 6.0;
   double dto6 = delta_t * one_sixth;
   double const * ER7_UTILS_RESTRICT veldot_0 = veldot_hist[0];
   double const * ER7_UTILS_RESTRICT veldot_1 = veldot_hist[1];
   double const * ER7_UTILS_RESTRICT veldot_2 = veldot_hist[2];

   for (int ii = 0; ii < size; ++ii) {
      double a1pa2 = veldot_1[ii] + veldot_2[ii];
      double mean_vel = init_vel[ii] + (veldot_0[ii] + a1pa2) * dto6;
      position[ii] = init_pos[ii] + mean_vel * delta_t;
      velocity[ii] = init_vel[ii] + (veldot_0[ii] + 2.0*a1pa2 + accel[ii])*dto6;
   }
}


// Propagate state for the special case of velocity being the derivative of
// position.
IntegratorResult
RKNystrom4SimpleSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   // Advance state per RKN4.
   switch (target_stage) {
   case 1:
      simple_rkn4_step_one (
         accel, 0.5*dyn_dt, state_size[0],
         init_pos, init_vel,
         veldot_hist[0], position, velocity);
      step_factor = 0.5;
      break;

   case 2:
#if 1
      integ_utils::euler_step_save_deriv (
         init_vel, accel, 0.5*dyn_dt, state_size[0],
         veldot_hist[1], velocity);
#else
      simple_rkn4_step_three (
         init_pos, init_vel, accel, 0.5*dyn_dt, state_size[0],
         veldot_hist[1], position, velocity);
#endif
      step_factor = 0.5;
      break;

   case 3:
      simple_rkn4_step_three (
         init_pos, init_vel, accel, dyn_dt, state_size[0],
         veldot_hist[2], position, velocity);
      step_factor = 1.0;
      break;

   case 4:
      simple_rkn4_step_four (
         init_pos, init_vel, accel, veldot_hist, dyn_dt, state_size[0],
         position, velocity);
      step_factor = 1.0;
      break;

   default:
      step_factor = 1.0;
      break;
   }

   return step_factor;
}


/**
 * Generalized step second order ODE fourth order Runge Kutta Nystrom step 1.
 * @param[in]     accel        Initial acceleration
 * @param[in]     hdt          Half of the integration time step
 * @param[in]     size         State size
 * @param[out]    init_vel     Initial velocity
 * @param[out]    dtheta       Position delta
 * @param[out]    init_veldot  Initial velocity derivative
 * @param[in,out] velocity     Updated velocity vector
 */
inline void ER7_UTILS_ALWAYS_INLINE
generalized_rkn4_step_one_velocity (
   double const * ER7_UTILS_RESTRICT accel,
   double hdt,
   int size,
   double * ER7_UTILS_RESTRICT init_vel,
   double * ER7_UTILS_RESTRICT dtheta,
   double * ER7_UTILS_RESTRICT init_veldot,
   double * ER7_UTILS_RESTRICT velocity)
{

   for (int ii = 0; ii < size; ++ii) {
      double adt = accel[ii]*hdt;
      dtheta[ii]       = (velocity[ii] + 0.5*adt)*hdt;
      init_vel[ii]     = velocity[ii];
      init_veldot[ii]  = accel[ii];
      velocity[ii]    += adt;
   }
}


/**
 * Generalized step second order ODE fourth order Runge Kutta Nystrom step 2/3.
 * @param[in]     init_vel     Initial velocity
 * @param[in]     accel        Initial acceleration
 * @param[in]     delta_t      Time step
 * @param[in]     size         State size
 * @param[out]    dtheta       Position delta
 * @param[out]    veldot       Saved velocity derivative
 * @param[in,out] velocity     Updated velocity vector
 */
inline void ER7_UTILS_ALWAYS_INLINE
generalized_rkn4_step_three_velocity (
   double const * ER7_UTILS_RESTRICT init_vel,
   double const * ER7_UTILS_RESTRICT accel,
   double delta_t,
   int size,
   double * ER7_UTILS_RESTRICT dtheta,
   double * ER7_UTILS_RESTRICT veldot,
   double * ER7_UTILS_RESTRICT velocity)
{
   for (int ii = 0; ii < size; ++ii) {
      double adt = accel[ii]*delta_t;
      dtheta[ii]   = (init_vel[ii] + 0.5*adt)*delta_t;
      veldot[ii]   = accel[ii];
      velocity[ii] = init_vel[ii] + adt;
   }
}


/**
 * Generalized step second order ODE fourth order Runge Kutta Nystrom step 4.
 * @param[in]     init_vel     Initial velocity
 * @param[in]     accel        Final acceleration
 * @param[out]    veldot_hist  Velocity derivatives history
 * @param[in]     delta_t      Integration time step
 * @param[in]     size         State size
 * @param[out]    dtheta       Position delta
 * @param[in,out] velocity     Updated velocity vector
 */
inline void ER7_UTILS_ALWAYS_INLINE
generalized_rkn4_step_four_velocity (
   double const * ER7_UTILS_RESTRICT init_vel,
   double const * ER7_UTILS_RESTRICT accel,
   double const * ER7_UTILS_RESTRICT const * ER7_UTILS_RESTRICT veldot_hist,
   double delta_t,
   int size,
   double * ER7_UTILS_RESTRICT dtheta,
   double * ER7_UTILS_RESTRICT velocity)
{
   static const double one_sixth = 1.0 / 6.0;
   double dto6 = delta_t * one_sixth;
   double const * ER7_UTILS_RESTRICT veldot_0 = veldot_hist[0];
   double const * ER7_UTILS_RESTRICT veldot_1 = veldot_hist[1];
   double const * ER7_UTILS_RESTRICT veldot_2 = veldot_hist[2];

   for (int ii = 0; ii < size; ++ii) {
      double a1pa2 = veldot_1[ii] + veldot_2[ii];
      dtheta[ii]   = (init_vel[ii] + (veldot_0[ii] + a1pa2) * dto6)*delta_t;
      velocity[ii] = init_vel[ii] + (veldot_0[ii] + 2.0*a1pa2 + accel[ii])*dto6;
   }
}


// Propagate state for the general case of the generalized position derivative
// being a function of generalized position and generalized velocity.
IntegratorResult
RKNystrom4GeneralizedStepSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   // Advance state per RKN4.
   switch (target_stage) {
   case 1:
      integ_utils::copy_array (position, state_size[0], init_pos);
      generalized_rkn4_step_one_velocity (
         accel, 0.5*dyn_dt, state_size[1],
         init_vel, dtheta, veldot_hist[0], velocity);
      compute_expmap_position_step (position, dtheta, position);
      step_factor = 0.5;
      break;

   case 2:
#if 0
      integ_utils::euler_step_save_deriv (
         init_vel, accel, 0.5*dyn_dt, state_size[0],
         veldot_hist[1], velocity);
#else
      generalized_rkn4_step_three_velocity (
         init_vel, accel, 0.5*dyn_dt, state_size[1],
         dtheta, veldot_hist[1], velocity);
      compute_expmap_position_step (init_pos, dtheta, position);
#endif
      step_factor = 0.5;
      break;

   case 3:
      generalized_rkn4_step_three_velocity (
         init_vel, accel, dyn_dt, state_size[1],
         dtheta, veldot_hist[2], velocity);
      compute_expmap_position_step (init_pos, dtheta, position);
      step_factor = 1.0;
      break;

   case 4:
      generalized_rkn4_step_four_velocity (
         init_vel, accel, veldot_hist, dyn_dt, state_size[1],
         dtheta, velocity);
      compute_expmap_position_step (init_pos, dtheta, position);
      step_factor = 1.0;
      break;

   default:
      step_factor = 1.0;
      break;
   }

   return step_factor;
}

}
