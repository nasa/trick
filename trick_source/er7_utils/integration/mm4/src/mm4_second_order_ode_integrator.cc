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
 * Defines member functions for the class MM4SecondOrderODEIntegrator.
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
#include "../include/mm4_second_order_ode_integrator.hh"


namespace er7_utils {

/**
 * Advance position and velocity for the 1st step of an MM4 integration cycle.
 * @param[in]     accel     Acceleration vector
 * @param[in]     hdt       Half of the integration interval time step
 * @param[in]     size      State size
 * @param[out]    pos_hist  Generalized position history
 * @param[out]    vel_hist  Generalized velocity history
 * @param[in,out] position  Generalized position
 * @param[in,out] velocity  Generalized velocity
 */
static inline void ER7_UTILS_ALWAYS_INLINE
second_order_mm4_step_one (
   double const * ER7_UTILS_RESTRICT accel,
   double hdt,
   int size,
   double * ER7_UTILS_RESTRICT * ER7_UTILS_RESTRICT pos_hist,
   double * ER7_UTILS_RESTRICT * ER7_UTILS_RESTRICT vel_hist,
   double * ER7_UTILS_RESTRICT position,
   double * ER7_UTILS_RESTRICT velocity)
{
   double * ER7_UTILS_RESTRICT init_pos = pos_hist[0];
   double * ER7_UTILS_RESTRICT init_vel = vel_hist[0];
   double * ER7_UTILS_RESTRICT step_pos = pos_hist[1];
   double * ER7_UTILS_RESTRICT step_vel = vel_hist[1];

   for (int ii = 0; ii < size; ++ii) {
      init_pos[ii] = position[ii];
      init_vel[ii] = velocity[ii];
      step_pos[ii] = (position[ii] += velocity[ii] * hdt);
      step_vel[ii] = (velocity[ii] += accel[ii] * hdt);
   }
}


/**
 * Advance position and velocity for the 1st step of an MM4 integration cycle.
 * @param[in]     posdot     Generalized position derivative
 * @param[in]     veldot     Generalized velocity derivative
 * @param[in]     hdt        Half of the integration interval time step
 * @param[in]     size       Sizes of the position and velocity vectors
 * @param[out]    pos_hist   Generalized position history
 * @param[out]    vel_hist   Generalized velocity history
 * @param[in,out] position   Generalized position
 * @param[in,out] velocity   Generalized velocity
 */
static inline void ER7_UTILS_ALWAYS_INLINE
second_order_mm4_step_one (
   double const * ER7_UTILS_RESTRICT posdot,
   double const * ER7_UTILS_RESTRICT veldot,
   double hdt,
   int size[2],
   double * ER7_UTILS_RESTRICT * ER7_UTILS_RESTRICT pos_hist,
   double * ER7_UTILS_RESTRICT * ER7_UTILS_RESTRICT vel_hist,
   double * ER7_UTILS_RESTRICT position,
   double * ER7_UTILS_RESTRICT velocity)
{
   double * ER7_UTILS_RESTRICT init_pos = pos_hist[0];
   double * ER7_UTILS_RESTRICT init_vel = vel_hist[0];
   double * ER7_UTILS_RESTRICT step_pos = pos_hist[1];
   double * ER7_UTILS_RESTRICT step_vel = vel_hist[1];
   int lim;

   lim = size[0];
   for (int ii = 0; ii < lim; ++ii) {
      init_pos[ii] = position[ii];
      step_pos[ii] = (position[ii] += posdot[ii] * hdt);
   }

   lim = size[1];
   for (int ii = 0; ii < lim; ++ii) {
      init_vel[ii] = velocity[ii];
      step_vel[ii] = (velocity[ii] += veldot[ii] * hdt);
   }
}


/**
 * Advance position and velocity for the 2nd step of an MM4 integration cycle.
 * @param[in]     accel     Acceleration vector
 * @param[in]     dt        Integration interval time step
 * @param[in]     size      State size
 * @param[in,out] pos_hist  Generalized position history
 * @param[in,out] vel_hist  Generalized velocity history
 * @param[out]    position  Generalized position
 * @param[out]    velocity  Generalized velocity
 */
static inline void ER7_UTILS_ALWAYS_INLINE
second_order_mm4_step_two (
   double const * ER7_UTILS_RESTRICT accel,
   double dt,
   int size,
   double * ER7_UTILS_RESTRICT * ER7_UTILS_RESTRICT pos_hist,
   double * ER7_UTILS_RESTRICT * ER7_UTILS_RESTRICT vel_hist,
   double * ER7_UTILS_RESTRICT position,
   double * ER7_UTILS_RESTRICT velocity)
{
   double * ER7_UTILS_RESTRICT init_pos = pos_hist[0];
   double * ER7_UTILS_RESTRICT init_vel = vel_hist[0];
   double * ER7_UTILS_RESTRICT step_pos = pos_hist[2];
   double * ER7_UTILS_RESTRICT step_vel = vel_hist[2];

   for (int ii = 0; ii < size; ++ii) {
      step_pos[ii] = position[ii] = init_pos[ii] + velocity[ii] * dt;
      step_vel[ii] = velocity[ii] = init_vel[ii] + accel[ii] * dt;
   }
}


/**
 * Advance position and velocity for the 2nd step of an MM4 integration cycle.
 * @param[in]     posdot    Generalized position derivative
 * @param[in]     veldot    Generalized velocity derivative
 * @param[in]     dt        Integration interval time step
 * @param[in]     size      State size
 * @param[in,out] pos_hist  Generalized position history
 * @param[in,out] vel_hist  Generalized velocity history
 * @param[out]    position  Generalized position
 * @param[out]    velocity  Generalized velocity
 */
static inline void ER7_UTILS_ALWAYS_INLINE
second_order_mm4_step_two (
   double const * ER7_UTILS_RESTRICT posdot,
   double const * ER7_UTILS_RESTRICT veldot,
   double dt,
   int size[2],
   double * ER7_UTILS_RESTRICT * ER7_UTILS_RESTRICT pos_hist,
   double * ER7_UTILS_RESTRICT * ER7_UTILS_RESTRICT vel_hist,
   double * ER7_UTILS_RESTRICT position,
   double * ER7_UTILS_RESTRICT velocity)
{
   double * ER7_UTILS_RESTRICT init_pos = pos_hist[0];
   double * ER7_UTILS_RESTRICT init_vel = vel_hist[0];
   double * ER7_UTILS_RESTRICT step_pos = pos_hist[2];
   double * ER7_UTILS_RESTRICT step_vel = vel_hist[2];
   int lim;

   lim = size[0];
   for (int ii = 0; ii < lim; ++ii) {
      step_pos[ii] = position[ii] = init_pos[ii] + posdot[ii] * dt;
   }

   lim = size[1];
   for (int ii = 0; ii < lim; ++ii) {
      step_vel[ii] = velocity[ii] = init_vel[ii] + veldot[ii] * dt;
   }
}


/**
 * Advance a state for the final step of an MM4 integration cycle.
 * @param deriv        Derivatives at final step
 * @param step1_state  State from end of first MM4 step
 * @param step2_state  State from end of second MM4 step
 * @param hdt          Half the integration interval time step
 * @param size         State size
 * @param state        Output state
 */
static inline void ER7_UTILS_ALWAYS_INLINE
first_order_mm4_step_three (
   double const * ER7_UTILS_RESTRICT deriv,
   double const * ER7_UTILS_RESTRICT step1_state,
   double const * ER7_UTILS_RESTRICT step2_state,
   double hdt,
   int size,
   double * ER7_UTILS_RESTRICT state)
{
   for (int ii = 0; ii < size; ++ii) {
      state[ii] = 0.5 * (step1_state[ii] + step2_state[ii] +
                         hdt*deriv[ii]);
   }
}


/**
 * Advance a state for the final step of an MM4 integration cycle.
 * @param accel      Acceleration at final step
 * @param step1_pos  Position from end of first MM4 step
 * @param step2_pos  Position from end of second MM4 step
 * @param step1_vel  Velocity from end of first MM4 step
 * @param step2_vel  Velocity from end of second MM4 step
 * @param hdt        Half the integration interval time step
 * @param size       State size
 * @param position   Output position
 * @param velocity   Output velocity
 */
static inline void ER7_UTILS_ALWAYS_INLINE
second_order_mm4_step_three (
   double const * ER7_UTILS_RESTRICT accel,
   double const * ER7_UTILS_RESTRICT step1_pos,
   double const * ER7_UTILS_RESTRICT step2_pos,
   double const * ER7_UTILS_RESTRICT step1_vel,
   double const * ER7_UTILS_RESTRICT step2_vel,
   double hdt,
   int size,
   double * ER7_UTILS_RESTRICT position,
   double * ER7_UTILS_RESTRICT velocity)
{
   for (int ii = 0; ii < size; ++ii) {
      velocity[ii] = 0.5 * (step1_vel[ii] + step2_vel[ii] + hdt*accel[ii]);
      position[ii] = 0.5 * (step1_pos[ii] + step2_pos[ii] + hdt*velocity[ii]);
   }
}


// Default constructor for an MM4SecondOrderODEIntegrator.
MM4SecondOrderODEIntegrator::MM4SecondOrderODEIntegrator (
   void)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (),
   posdot (NULL)
{
   // Initialize memory used by MM4 algorithm.
   alloc::initialize_2D_array<3> (pos_hist);
   alloc::initialize_2D_array<3> (vel_hist);
}


// Copy constructor.
MM4SecondOrderODEIntegrator::MM4SecondOrderODEIntegrator (
   const MM4SecondOrderODEIntegrator & src)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (src),
   posdot (NULL)
{
   // Replicate the source's contents if they exist.
   if (src.posdot != NULL) {
      posdot = alloc::replicate_array (state_size[0], src.posdot);
      alloc::replicate_2D_array<3> (state_size[0], src.pos_hist, pos_hist);
      alloc::replicate_2D_array<3> (state_size[1], src.vel_hist, vel_hist);
   }

   else {
      alloc::initialize_2D_array<3> (pos_hist);
      alloc::initialize_2D_array<3> (vel_hist);
   }
}


// Non-default constructor for an MM4SecondOrderODEIntegrator
MM4SecondOrderODEIntegrator::MM4SecondOrderODEIntegrator (
   unsigned int size,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (size, controls),
   posdot (NULL)
{
   // Allocate memory used by MM4 algorithm.
   alloc::allocate_2D_array<3> (size, pos_hist);
   alloc::allocate_2D_array<3> (size, vel_hist);
   posdot = alloc::allocate_array (size);
}


// Non-default constructor for an MM4SecondOrderODEIntegrator
// for generalized position, generalized velocity.
MM4SecondOrderODEIntegrator::MM4SecondOrderODEIntegrator (
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
   // Allocate memory used by MM4 algorithm.
   alloc::allocate_2D_array<3> (position_size, pos_hist);
   alloc::allocate_2D_array<3> (velocity_size, vel_hist);
   posdot = alloc::allocate_array (position_size);
}


// MM4SecondOrderODEIntegrator destructor.
MM4SecondOrderODEIntegrator::~MM4SecondOrderODEIntegrator (
   void)
{
   alloc::deallocate_2D_array<3> (pos_hist);
   alloc::deallocate_2D_array<3> (vel_hist);
   alloc::deallocate_array (posdot);
}


// Non-throwing swap.
void
MM4SecondOrderODEIntegrator::swap (
   MM4SecondOrderODEIntegrator & other)
{
   SecondOrderODEIntegrator::swap (other);

   std::swap (posdot, other.posdot);
   for (int ii = 0; ii < 4; ++ii) {
      std::swap (pos_hist[ii], other.pos_hist[ii]);
      std::swap (vel_hist[ii], other.vel_hist[ii]);
   }
}


// Clone a MM4SimpleSecondOrderODEIntegrator.
MM4SimpleSecondOrderODEIntegrator *
MM4SimpleSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Clone a MM4GeneralizedDerivSecondOrderODEIntegrator.
MM4GeneralizedDerivSecondOrderODEIntegrator *
MM4GeneralizedDerivSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Propagate state for the special case of velocity being the derivative of
// position.
IntegratorResult
MM4SimpleSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   switch (target_stage) {
   case 1:
      second_order_mm4_step_one (
         accel, 0.5*dyn_dt, state_size[0],
         pos_hist, vel_hist, position, velocity);
      step_factor = 0.5;
      break;

   case 2:
      second_order_mm4_step_two (
         accel, dyn_dt, state_size[0],
         pos_hist, vel_hist, position, velocity);
      step_factor = 1.0;
      break;

   case 3:
      second_order_mm4_step_three (
         accel, pos_hist[1], pos_hist[2], vel_hist[1], vel_hist[2],
         0.5*dyn_dt, state_size[0],
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
MM4GeneralizedDerivSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   switch (target_stage) {
   case 1:
      compute_posdot (position, velocity, posdot);
      second_order_mm4_step_one (
         posdot, accel, 0.5*dyn_dt, state_size,
         pos_hist, vel_hist, position, velocity);
      step_factor = 0.5;
      break;

   case 2:
      compute_posdot (position, velocity, posdot);
      second_order_mm4_step_two (
         posdot, accel, dyn_dt, state_size,
         pos_hist, vel_hist, position, velocity);
      step_factor = 1.0;
      break;

   case 3: {
      double hdt = 0.5 * dyn_dt;
      first_order_mm4_step_three (
         accel, vel_hist[1], vel_hist[2], hdt, state_size[1], velocity);
      compute_posdot (position, velocity, posdot);
      first_order_mm4_step_three (
         posdot, pos_hist[1], pos_hist[2], hdt, state_size[0], position);
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
