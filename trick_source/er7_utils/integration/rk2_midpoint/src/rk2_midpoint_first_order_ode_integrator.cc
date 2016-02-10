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
 * Defines member functions for the class RK2MidpointFirstOrderODEIntegrator.
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
#include "../include/rk2_midpoint_first_order_ode_integrator.hh"


namespace er7_utils {

// RK2MidpointFirstOrderODEIntegrator default constructor.
RK2MidpointFirstOrderODEIntegrator::RK2MidpointFirstOrderODEIntegrator (
   void)
:
   Er7UtilsDeletable (),
   FirstOrderODEIntegrator (),
   init_state (NULL)
{
}


// Copy constructor.
RK2MidpointFirstOrderODEIntegrator::RK2MidpointFirstOrderODEIntegrator (
   const RK2MidpointFirstOrderODEIntegrator & src)
:
   Er7UtilsDeletable (),
   FirstOrderODEIntegrator (src),
   init_state (NULL)
{
   // Replicate the source's contents if they exist.
   if (src.init_state != NULL) {
      init_state = alloc::replicate_array<double> (state_size, src.init_state);
   }
}


// RK2MidpointFirstOrderODEIntegrator non-default constructor.
RK2MidpointFirstOrderODEIntegrator::RK2MidpointFirstOrderODEIntegrator (
   unsigned int size,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   FirstOrderODEIntegrator (size, controls),
   init_state (NULL)
{
   // Allocate memory used by Runge Kutta 2 algorithm.
   init_state = alloc::allocate_array<double> (state_size);
}


// RK2MidpointFirstOrderODEIntegrator destructor.
RK2MidpointFirstOrderODEIntegrator::~RK2MidpointFirstOrderODEIntegrator (
   void)
{
   alloc::deallocate_array<double> (init_state);
}


// Clone a RK2MidpointFirstOrderODEIntegrator.
RK2MidpointFirstOrderODEIntegrator *
RK2MidpointFirstOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Non-throwing swap.
void
RK2MidpointFirstOrderODEIntegrator::swap (
   RK2MidpointFirstOrderODEIntegrator & other)
{
   FirstOrderODEIntegrator::swap (other);
   std::swap (init_state, other.init_state);
}


// Propagate state via second order Runga Kutta.
IntegratorResult
RK2MidpointFirstOrderODEIntegrator::integrate (
   double dt,
   unsigned int target_stage,
   const double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   switch (target_stage) {
   case 1:
      integ_utils::inplace_euler_step_save_state (
         velocity, 0.5*dt, state_size, init_state, position);
      step_factor = 0.5;
      break;

   case 2:
      integ_utils::euler_step (init_state, velocity, dt, state_size, position);
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
