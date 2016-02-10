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
 * Defines the class RK4FirstOrderODEIntegrator, which integrates a monolithic
 * state via the canonical fourth order Runge Kutta method.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_RK4_ONE_STATE_INTEGRATOR_HH
#define ER7_UTILS_RK4_ONE_STATE_INTEGRATOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/first_order_ode_integrator.hh"


namespace er7_utils {

/**
 * Propagate state using the standard 4th order Runge-Kutta method.
 */
class RK4FirstOrderODEIntegrator : public FirstOrderODEIntegrator {
ER7_UTILS_MAKE_SIM_INTERFACES(RK4FirstOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RK4FirstOrderODEIntegrator default constructor.
    */
   RK4FirstOrderODEIntegrator (void);

   /**
    * RK4FirstOrderODEIntegrator copy constructor.
    * @param[in] src  Item to be copied.
    */
   RK4FirstOrderODEIntegrator (const RK4FirstOrderODEIntegrator & src);

   /**
    * RK4FirstOrderODEIntegrator non-default constructor.
    * @param[in]     size      State size
    * @param[in,out] controls  Integration controls
    */
   RK4FirstOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls);

   /**
    * RK4FirstOrderODEIntegrator destructor.
    */
   virtual ~RK4FirstOrderODEIntegrator (void);


   // Member functions.

   /**
    * RK4FirstOrderODEIntegrator assignment operator.
    * @param src  Item to be copied.
    */
   RK4FirstOrderODEIntegrator & operator=(
      RK4FirstOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' RK4FirstOrderODEIntegrator object.
    * @return Clone of 'this'.
    */
   virtual RK4FirstOrderODEIntegrator * create_copy () const;

   /**
    * Propagate state via the standard RK4 integration scheme.
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
   void swap (RK4FirstOrderODEIntegrator & other);

   using FirstOrderODEIntegrator::swap;


   // Member data.

   double * init_state; /**< trick_units(--) @n
      State at the start of an integration cycle. */

   double * deriv_hist[4]; /**< trick_units(--) @n
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
