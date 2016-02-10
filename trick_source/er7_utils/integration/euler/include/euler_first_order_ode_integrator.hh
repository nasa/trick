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
 * Defines the class EulerFirstOrderODEIntegrator, which integrates a monolithic
 * state via the Euler method.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_EULER_ONE_STATE_INTEGRATOR_HH
#define ER7_UTILS_EULER_ONE_STATE_INTEGRATOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/first_order_ode_integrator.hh"


namespace er7_utils {

/**
 * Propagate state using Euler integration.
 */
class EulerFirstOrderODEIntegrator : public FirstOrderODEIntegrator {
ER7_UTILS_MAKE_SIM_INTERFACES(EulerFirstOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * EulerFirstOrderODEIntegrator default constructor.
    */
   EulerFirstOrderODEIntegrator (void);

   /**
    * EulerFirstOrderODEIntegrator copy constructor.
    * @param[in] src  Item to be copied.
    */
   EulerFirstOrderODEIntegrator (const EulerFirstOrderODEIntegrator & src);

   /**
    * EulerFirstOrderODEIntegrator non-default constructor.
    * @param[in]     size      State size
    * @param[in,out] controls  Integration controls
    */
   EulerFirstOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls);

   /**
    * EulerFirstOrderODEIntegrator destructor.
    */
   virtual ~EulerFirstOrderODEIntegrator (void);


   // Member functions.

   /**
    * EulerFirstOrderODEIntegrator assignment operator.
    * @param src  Item to be copied.
    */
   EulerFirstOrderODEIntegrator & operator=(
      EulerFirstOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' RK4FirstOrderODEIntegrator object.
    * @return Clone of 'this'.
    */
   virtual EulerFirstOrderODEIntegrator * create_copy () const;

   /**
    * Propagate state via Euler's method.
    * @param[in] dyn_dt        Integration interval step, dynamic time seconds.
    * @param[in] target_stage  The stage of the integration process
    *                          that the integrator should try to attain.
    * @param[in,out] velocity  Generalized velocity vector.
    * @param[in,out] position  Generalized position vector.
    *
    * @return The status (time advance, pass/fail status) of the integration.
    */
   virtual IntegratorResult integrate (
      double dyn_dt,
      unsigned int target_stage,
      const double * ER7_UTILS_RESTRICT velocity,
      double * ER7_UTILS_RESTRICT position);

};

}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
