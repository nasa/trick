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
 * Defines the class RKFehlberg78IntegratorConstructor, which constructs
 * integrators that use RKFehlberg78 integration.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_RKF78_INTEGRATOR_CONSTRUCTOR_HH
#define ER7_UTILS_RKF78_INTEGRATOR_CONSTRUCTOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/integrator_constructor.hh"


namespace er7_utils {

/**
 * Create state and time integrators that propagate using RKFehlberg78.
 */
class RKFehlberg78IntegratorConstructor : public IntegratorConstructor {
ER7_UTILS_MAKE_SIM_INTERFACES(RKFehlberg78IntegratorConstructor)

   // Note: The default constructor, copy constructor, assignment operator,
   // and destructor for this class are not declared.
   // The implicit C++ defaults suffice for this class.

public:
   // Static member functions.

   /**
    * Named constructor; create an RKFehlberg78IntegratorConstructor instance.
    * The caller is responsible for deleting the returned object.
    * @return Newly created RKFehlberg78IntegratorConstructor instance.
    */
   static IntegratorConstructor* create_constructor (void);


   // Member functions.

   /**
    * Return the class name.
    */
   virtual const char * get_class_name (void) const
   { return "RKFehlberg78IntegratorConstructor"; }

   /**
    * Create a duplicate of the constructor.
    * The caller is responsible for deleting the returned object.
    * @return Duplicated constructor.
    */
   virtual IntegratorConstructor * create_copy (void) const;

   /**
    * Create an integration controls that guides the RKF7/8 integration process.
    * The caller is responsible for deleting the created object.
    * @return Integration controls object
    */
   virtual IntegrationControls * create_integration_controls (void) const;

   /**
    * Create an RKFehlberg78 state integrator for a first order ODE.
    * The caller is responsible for deleting the created object.
    * @return State integrator
    * @param[in]     size      State size
    * @param[in,out] controls  Integration controls
    */
   virtual FirstOrderODEIntegrator * create_first_order_ode_integrator (
      unsigned int size,
      IntegrationControls & controls) const;

   /**
    * Create an RKFehlberg78 state integrator for a simple second order ODE.
    * The caller is responsible for deleting the created object.
    * @return State integrator
    * @param[in]     size      State size
    * @param[in,out] controls  Integration controls
    */
   virtual SecondOrderODEIntegrator * create_second_order_ode_integrator (
      unsigned int size,
      IntegrationControls & controls) const;

   /**
    * Create an RKFehlberg78 state integrator for a generalized second order ODE
    * where generalized position is advanced with the use of the
    * position derivative function.
    * The caller is responsible for deleting the created object.
    * @return State integrator
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Position derivative functions container
    * @param[in,out] controls       Integration controls
    */
   virtual SecondOrderODEIntegrator *
   create_generalized_deriv_second_order_ode_integrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls) const;

   /**
    * Create an RKFehlberg78 state integrator for a generalized second order ODE
    * where generalized position is advanced with the use of the
    * position step function.
    * The caller is responsible for deleting the created object.
    * @return State integrator
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     step_funs      Position step functions container
    * @param[in,out] controls       Integration controls
    */
   virtual SecondOrderODEIntegrator *
   create_generalized_step_second_order_ode_integrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls) const;


   /**
    * RKFehlberg78 uses thirteen steps per cycle.
    * @return Always returns 13.
    */
   virtual unsigned int get_transition_table_size (void) const
   { return 13; }
};

}


#ifdef ER7_UTILS_NEED_AUX_INCLUDES
#include "er7_utils/integration/core/include/single_cycle_integration_controls.hh"
#include "rkf78_first_order_ode_integrator.hh"
#include "rkf78_second_order_ode_integrator.hh"
#endif


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
