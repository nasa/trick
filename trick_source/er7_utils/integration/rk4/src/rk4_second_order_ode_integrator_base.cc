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
 * Defines member functions for the class RK4SecondOrderODEIntegrator.
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

// Model includes
#include "../include/rk4_second_order_ode_integrator_base.hh"


namespace er7_utils {

// RK4SecondOrderODEIntegrator default constructor.
RK4SecondOrderODEIntegrator::RK4SecondOrderODEIntegrator (
   void)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (),
   init_pos (NULL),
   init_vel (NULL),
   saved_pos (NULL),
   dtheta (NULL)
{
   // Initialize memory used by Runge Kutta 4 algorithm.
   alloc::initialize_2D_array<4> (posdot_hist);
   alloc::initialize_2D_array<4> (veldot_hist);
}


// Copy constructor.
RK4SecondOrderODEIntegrator::RK4SecondOrderODEIntegrator (
   const RK4SecondOrderODEIntegrator & src)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (src),
   init_pos (NULL),
   init_vel (NULL),
   saved_pos (NULL),
   dtheta (NULL)
{
   // Replicate the source's contents if they exist.
   if (problem_type == Integration::GeneralizedStepSecondOrderODE) {
      init_pos  = alloc::replicate_array (state_size[0], src.init_pos);
      init_vel  = alloc::replicate_array (state_size[1], src.init_vel);
      saved_pos = alloc::replicate_array (state_size[0], src.saved_pos);
      dtheta    = alloc::replicate_array (state_size[1], src.dtheta);
      alloc::replicate_2D_array<4> (state_size[1], src.posdot_hist,
                                    posdot_hist);
      alloc::replicate_2D_array<4> (state_size[1], src.veldot_hist,
                                    veldot_hist);
   }

   else if (src.init_pos != NULL) {
      init_pos = alloc::replicate_array (state_size[0], src.init_pos);
      init_vel = alloc::replicate_array (state_size[1], src.init_vel);
      alloc::replicate_2D_array<4> (state_size[0], src.posdot_hist,
                                    posdot_hist);
      alloc::replicate_2D_array<4> (state_size[1], src.veldot_hist,
                                    veldot_hist);
   }

   else {
      alloc::initialize_2D_array<4> (posdot_hist);
      alloc::initialize_2D_array<4> (veldot_hist);
   }
}


// Non-default constructor for an RK4SecondOrderODEIntegrator
// in which the time derivative of position is the generalized velocity.
RK4SecondOrderODEIntegrator::RK4SecondOrderODEIntegrator (
   unsigned int size,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (size, controls),
   init_pos (NULL),
   init_vel (NULL),
   saved_pos (NULL),
   dtheta (NULL)
{
   // Allocate memory used by Runge Kutta 4 algorithm.
   init_pos = alloc::allocate_array (size);
   init_vel = alloc::allocate_array (size);
   alloc::allocate_2D_array<4> (size, posdot_hist);
   alloc::allocate_2D_array<4> (size, veldot_hist);
}


// Non-default constructor for an RK4SecondOrderODEIntegrator
// for generalized position, generalized velocity.
RK4SecondOrderODEIntegrator::RK4SecondOrderODEIntegrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionDerivativeFunctions & deriv_funs,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (position_size, velocity_size,
                             deriv_funs, controls),
   init_pos (NULL),
   init_vel (NULL),
   saved_pos (NULL),
   dtheta (NULL)
{
   // Allocate memory used by Runge Kutta 4 algorithm.
   init_pos = alloc::allocate_array (position_size);
   init_vel = alloc::allocate_array (velocity_size);
   alloc::allocate_2D_array<4> (position_size, posdot_hist);
   alloc::allocate_2D_array<4> (velocity_size, veldot_hist);
}


// Non-default constructor for an RK4SecondOrderODEIntegrator
// for generalized position, generalized velocity.
RK4SecondOrderODEIntegrator::RK4SecondOrderODEIntegrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionStepFunctions & step_funs,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (position_size, velocity_size,
                             step_funs, controls),
   init_pos (NULL),
   init_vel (NULL),
   saved_pos (NULL),
   dtheta (NULL)
{
   // Allocate memory used by Runge Kutta 4 algorithm.
   init_pos  = alloc::allocate_array (position_size);
   init_vel  = alloc::allocate_array (velocity_size);
   saved_pos = alloc::allocate_array (position_size);
   dtheta    = alloc::allocate_array (velocity_size);
   alloc::allocate_2D_array<4> (velocity_size, posdot_hist);
   alloc::allocate_2D_array<4> (velocity_size, veldot_hist);
}


// RK4SecondOrderODEIntegrator destructor.
RK4SecondOrderODEIntegrator::~RK4SecondOrderODEIntegrator (
   void)
{
   alloc::deallocate_array (init_pos);
   alloc::deallocate_array (init_vel);
   alloc::deallocate_array (saved_pos);
   alloc::deallocate_array (dtheta);
   alloc::deallocate_2D_array<4> (posdot_hist);
   alloc::deallocate_2D_array<4> (veldot_hist);
}


// Non-throwing swap.
void
RK4SecondOrderODEIntegrator::swap (
   RK4SecondOrderODEIntegrator & other)
{
   SecondOrderODEIntegrator::swap (other);

   std::swap (init_pos, other.init_pos);
   std::swap (init_vel, other.init_vel);
   std::swap (saved_pos, other.saved_pos);
   std::swap (dtheta, other.dtheta);
   for (int ii = 0; ii < 4; ++ii) {
      std::swap (posdot_hist[ii], other.posdot_hist[ii]);
      std::swap (veldot_hist[ii], other.veldot_hist[ii]);
   }
}

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
