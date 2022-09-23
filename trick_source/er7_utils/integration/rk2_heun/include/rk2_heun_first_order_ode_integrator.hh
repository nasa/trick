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
 * Defines the class RK2HeunFirstOrderODEIntegrator, which integrates a
 * monolithic state via Heun's method.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_RK2_HEUN_ONE_STATE_INTEGRATOR_HH
#define ER7_UTILS_RK2_HEUN_ONE_STATE_INTEGRATOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/first_order_ode_integrator.hh"


namespace er7_utils {

/**
 * Propagate state using Heun's method.
 */
class RK2HeunFirstOrderODEIntegrator : public FirstOrderODEIntegrator {
ER7_UTILS_MAKE_SIM_INTERFACES(RK2HeunFirstOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RK2HeunFirstOrderODEIntegrator default constructor.
    */
   RK2HeunFirstOrderODEIntegrator (void);

   /**
    * RK2HeunFirstOrderODEIntegrator copy constructor.
    * @param[in] src  Item to be copied.
    */
   RK2HeunFirstOrderODEIntegrator (
      const RK2HeunFirstOrderODEIntegrator & src);

   /**
    * RK2HeunFirstOrderODEIntegrator non-default constructor.
    * @param[in]     size      State size
    * @param[in,out] controls  Integration controls
    */
   RK2HeunFirstOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls);

   /**
    * RK2HeunFirstOrderODEIntegrator destructor.
    */
   virtual ~RK2HeunFirstOrderODEIntegrator (void);


   // Member functions.

   /**
    * RK2HeunFirstOrderODEIntegrator assignment operator.
    * @param src  Item to be copied.
    */
   RK2HeunFirstOrderODEIntegrator & operator=(
      RK2HeunFirstOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' RK2HeunFirstOrderODEIntegrator object.
    * @return Clone of 'this'.
    */
   virtual RK2HeunFirstOrderODEIntegrator * create_copy () const;

   /**
    * Propagate state via second order Heun's method.
    * @param[in]     dyn_dt        Integration interval, dynamic time seconds.
    * @param[in]     target_stage  The stage of the integration process
    *                              that the integrator should try to attain.
    * @param[in,out] velocity      Generalized velocity vector.
    * @param[in,out] position      Generalized position vector.
    *
    * @return The status (time advance, pass/fail status) of the integration.
    */
   virtual IntegratorResult integrate (
      double dyn_dt,
      unsigned int target_stage,
      const double * ER7_UTILS_RESTRICT velocity,
      double * ER7_UTILS_RESTRICT position);


protected:

   // Member functions.

   /**
    * Non-throwing swap.
    * @param other  Item with which contents are to be swapped.
    */
   void swap (RK2HeunFirstOrderODEIntegrator & other);

   using FirstOrderODEIntegrator::swap;


   // Member data.

   double * init_state; /**< trick_units(--) @n
      State at the start of an integration cycle. */

   double * init_deriv; /**< trick_units(--) @n
      State derivative at the start of an integration cycle. */
};

}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
