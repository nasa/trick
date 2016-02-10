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
 * Defines member functions for the class EulerFirstOrderODEIntegrator.
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
#include "../include/euler_first_order_ode_integrator.hh"


namespace er7_utils {

// Default constructor for a EulerFirstOrderODEIntegrator.
EulerFirstOrderODEIntegrator::EulerFirstOrderODEIntegrator (
   void)
:
   Er7UtilsDeletable (),
   FirstOrderODEIntegrator ()
{
   ; /* Empty */
}


// EulerFirstOrderODEIntegrator copy constructor.
EulerFirstOrderODEIntegrator::EulerFirstOrderODEIntegrator (
   const EulerFirstOrderODEIntegrator & src)
:
   Er7UtilsDeletable (),
   FirstOrderODEIntegrator (src)
{
   ; /* Empty */
}


// Non-default constructor for a EulerFirstOrderODEIntegrator.
EulerFirstOrderODEIntegrator::EulerFirstOrderODEIntegrator (
   unsigned int size,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   FirstOrderODEIntegrator (size, controls)
{
   ; /* Empty */
}


// Destructor for a EulerFirstOrderODEIntegrator.
EulerFirstOrderODEIntegrator::~EulerFirstOrderODEIntegrator (
   void)
{
   ; /* Empty */
}


// Clone a EulerFirstOrderODEIntegrator.
EulerFirstOrderODEIntegrator *
EulerFirstOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Propagate state via Euler's method.
IntegratorResult
EulerFirstOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   const double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{

   // The only valid target_stage is 1.
   if (target_stage == 1) {
      integ_utils::inplace_euler_step (velocity, dyn_dt, state_size, position);
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
