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
 * Defines member functions for the class RK2HeunFirstOrderODEIntegrator.
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
#include "../include/rk2_heun_first_order_ode_integrator.hh"


namespace er7_utils {

// RK2HeunFirstOrderODEIntegrator default constructor.
RK2HeunFirstOrderODEIntegrator::RK2HeunFirstOrderODEIntegrator (
   void)
:
   Er7UtilsDeletable (),
   FirstOrderODEIntegrator (),
   init_state (NULL),
   init_deriv (NULL)
{
}


// Copy constructor.
RK2HeunFirstOrderODEIntegrator::RK2HeunFirstOrderODEIntegrator (
   const RK2HeunFirstOrderODEIntegrator & src)
:
   Er7UtilsDeletable (),
   FirstOrderODEIntegrator (src),
   init_state (NULL),
   init_deriv (NULL)
{
   // Replicate the source's contents if they exist.
   if (src.init_state != NULL) {
      init_state = alloc::replicate_array<double> (state_size, src.init_state);
      init_deriv = alloc::replicate_array<double> (state_size, src.init_deriv);
   }
}


// RK2HeunFirstOrderODEIntegrator non-default constructor.
RK2HeunFirstOrderODEIntegrator::RK2HeunFirstOrderODEIntegrator (
   unsigned int size,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   FirstOrderODEIntegrator (size, controls),
   init_state (NULL),
   init_deriv (NULL)
{
   // Allocate memory used by Runge Kutta 2 algorithm.
   init_state = alloc::allocate_array<double> (state_size);
   init_deriv = alloc::allocate_array<double> (state_size);
}


// RK2HeunFirstOrderODEIntegrator destructor.
RK2HeunFirstOrderODEIntegrator::~RK2HeunFirstOrderODEIntegrator (
   void)
{
   alloc::deallocate_array<double> (init_state);
   alloc::deallocate_array<double> (init_deriv);
}


// Clone a RK2HeunFirstOrderODEIntegrator.
RK2HeunFirstOrderODEIntegrator *
RK2HeunFirstOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Non-throwing swap.
void
RK2HeunFirstOrderODEIntegrator::swap (
   RK2HeunFirstOrderODEIntegrator & other)
{
   FirstOrderODEIntegrator::swap (other);
   std::swap (init_state, other.init_state);
   std::swap (init_deriv, other.init_deriv);
}


// Propagate state via Heun's method.
IntegratorResult
RK2HeunFirstOrderODEIntegrator::integrate (
   double dt,
   unsigned int target_stage,
   const double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   switch (target_stage) {
   case 1:
      integ_utils::inplace_euler_step_save_both (
         velocity, dt, state_size,
         init_state, init_deriv, position);
      break;

   case 2:
      rk::rk2_heun_final_step (
         init_state, init_deriv, velocity, dt, state_size,
         position);
      break;
   }

   return 1.0;
}

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
