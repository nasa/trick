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
 * Defines the class RK2MidpointFirstOrderODEIntegrator, which integrates a
 * monolithic state via the RK2 midpoint method.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_RK2_MIDPOINT_ONE_STATE_INTEGRATOR_HH
#define ER7_UTILS_RK2_MIDPOINT_ONE_STATE_INTEGRATOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/first_order_ode_integrator.hh"


namespace er7_utils {

/**
 * Propagate state using the RK2 midpoint method.
 */
class RK2MidpointFirstOrderODEIntegrator : public FirstOrderODEIntegrator {
ER7_UTILS_MAKE_SIM_INTERFACES(RK2MidpointFirstOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RK2MidpointFirstOrderODEIntegrator default constructor.
    */
   RK2MidpointFirstOrderODEIntegrator (void);

   /**
    * RK2MidpointFirstOrderODEIntegrator copy constructor.
    * @param[in] src  Item to be copied.
    */
   RK2MidpointFirstOrderODEIntegrator (
      const RK2MidpointFirstOrderODEIntegrator & src);

   /**
    * RK2MidpointFirstOrderODEIntegrator non-default constructor.
    * @param[in]     size      State size
    * @param[in,out] controls  Integration controls
    */
   RK2MidpointFirstOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls);

   /**
    * RK2MidpointFirstOrderODEIntegrator destructor.
    */
   virtual ~RK2MidpointFirstOrderODEIntegrator (void);


   // Member functions.

   /**
    * RK2MidpointFirstOrderODEIntegrator assignment operator.
    * @param src  Item to be copied.
    */
   RK2MidpointFirstOrderODEIntegrator & operator=(
      RK2MidpointFirstOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' RK2MidpointFirstOrderODEIntegrator object.
    * @return Clone of 'this'.
    */
   virtual RK2MidpointFirstOrderODEIntegrator * create_copy () const;

   /**
    * Propagate state via RK2 midpoint.
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


protected:

   // Member functions.

   /**
    * Non-throwing swap.
    * @param other  Item with which contents are to be swapped.
    */
   void swap (RK2MidpointFirstOrderODEIntegrator & other);

   using FirstOrderODEIntegrator::swap;


   // Member data.

   double * init_state; /**< trick_units(--) @n
      State at the start of an integration cycle. */

};

}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
