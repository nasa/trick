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
 * Defines the class RKFehlberg45FirstOrderODEIntegrator, which integrates a
 * monolithic state via the Runge-Kutta Fehlberg 4/5 method.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_RKF45_ONE_STATE_INTEGRATOR_HH
#define ER7_UTILS_RKF45_ONE_STATE_INTEGRATOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/first_order_ode_integrator.hh"


namespace er7_utils {

/**
 * Propagate state using the Runge-Kutta Fehlberg 4/5 method.
 */
class RKFehlberg45FirstOrderODEIntegrator : public FirstOrderODEIntegrator {
ER7_UTILS_MAKE_SIM_INTERFACES(RKFehlberg45FirstOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RKFehlberg45FirstOrderODEIntegrator default constructor.
    */
   RKFehlberg45FirstOrderODEIntegrator (void);

   /**
    * RKFehlberg45FirstOrderODEIntegrator copy constructor.
    * @param[in] src  Item to be copied.
    */
   RKFehlberg45FirstOrderODEIntegrator (
      const RKFehlberg45FirstOrderODEIntegrator & src);

   /**
    * RKFehlberg45FirstOrderODEIntegrator non-default constructor.
    * @param[in]     size      State size
    * @param[in,out] controls  Integration controls
    */
   RKFehlberg45FirstOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls);

   /**
    * RKFehlberg45FirstOrderODEIntegrator destructor.
    */
   virtual ~RKFehlberg45FirstOrderODEIntegrator (void);


   // Member functions.

   /**
    * RKFehlberg45FirstOrderODEIntegrator assignment operator.
    * @param src  Item to be copied.
    */
   RKFehlberg45FirstOrderODEIntegrator & operator=(
      RKFehlberg45FirstOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' RKFehlberg45FirstOrderODEIntegrator object.
    * @return Clone of 'this'.
    */
   virtual RKFehlberg45FirstOrderODEIntegrator * create_copy () const;

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
   void swap (RKFehlberg45FirstOrderODEIntegrator & other);

   using FirstOrderODEIntegrator::swap;


   // Member data.

   double * init_state; /**< trick_units(--) @n
      State at the start of an integration cycle. */

   double * deriv_hist[6]; /**< trick_units(--) @n
      State derivatives at each step in the integration cycle. */
};

}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
