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
 * Defines the class ABM4IntegratorConstructor, which constructs integrators
 * that use ABM4 integration.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_ABM4_INTEGRATOR_CONSTRUCTOR_HH
#define ER7_UTILS_ABM4_INTEGRATOR_CONSTRUCTOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/priming_integrator_constructor.hh"
#include "er7_utils/integration/rk4/include/rk4_integrator_constructor.hh"


namespace er7_utils {

/**
 * Create state and time integrators that propagate using fourth order
 * Adams-Bashforth-Moulton.
 */
class ABM4IntegratorConstructor : public PrimingIntegratorConstructor {
ER7_UTILS_MAKE_SIM_INTERFACES(ABM4IntegratorConstructor)

public:
   // Static member functions.

   /**
    * Named constructor; create an ABM4IntegratorConstructor instance.
    * The caller is responsible for deleting the returned object.
    * @return Newly created ABM4IntegratorConstructor instance.
    */
   static IntegratorConstructor* create_constructor (void);


   // Constructors and assignment operator
   // Note that the destructor is not declared.
   // The implicitly defined destructor is correct.

   /**
    * ABM4 default constructor.
    * Note that by default ABM4 uses RK4 for priming.
    */
   ABM4IntegratorConstructor (void)
   :
      Er7UtilsDeletable (),
      PrimingIntegratorConstructor (RK4IntegratorConstructor ())
   {}

   /**
    * ABM4IntegratorConstructor copy constructor.
    */
   ABM4IntegratorConstructor (const ABM4IntegratorConstructor & src)
   :
      Er7UtilsDeletable (),
      PrimingIntegratorConstructor (src)
   {}

   /**
    * ABM4IntegratorConstructor assignment operator.
    */
   ABM4IntegratorConstructor & operator= (ABM4IntegratorConstructor src)
   {
      swap (src);
      return *this;
   }


   // Member functions.

   /**
    * Return the class name.
    */
   virtual const char * get_class_name (void) const
   { return "ABM4IntegratorConstructor"; }

   /**
    * ABM4 currently does not implement a second order generalized step
    * integrator.
    */
   virtual bool implements (Integration::ODEProblemType problem_type) const
   {
      return (problem_type != Integration::GeneralizedStepSecondOrderODE);
   }

   /**
    * ABM4 currently does not provide a second order generalized step
    * integrator.
    */
   virtual bool provides (Integration::ODEProblemType problem_type) const
   {
      return (problem_type != Integration::GeneralizedStepSecondOrderODE);
   }

   /**
    * Create a duplicate of the constructor.
    * The caller is responsible for deleting the returned object.
    * @return Duplicated constructor.
    */
   virtual IntegratorConstructor * create_copy (void) const;

   /**
    * Create an integration controls that guides the ABM4 integration process.
    * The caller is responsible for deleting the created object.
    * @return Integration controls object
    */
   virtual IntegrationControls * create_integration_controls (void) const;

   /**
    * Create an ABM4 state integrator for a first order ODE.
    * The caller is responsible for deleting the created object.
    * @return State integrator
    * @param[in]     size      State size
    * @param[in,out] controls  Integration controls
    */
   virtual FirstOrderODEIntegrator * create_first_order_ode_integrator (
      unsigned int size,
      IntegrationControls & controls) const;

   /**
    * Create an ABM4 state integrator for a simple second order ODE.
    * The caller is responsible for deleting the created object.
    * @return State integrator
    * @param[in]     size      State size
    * @param[in,out] controls  Integration controls
    */
   virtual SecondOrderODEIntegrator * create_second_order_ode_integrator (
      unsigned int size,
      IntegrationControls & controls) const;

   /**
    * Create an ABM4 state integrator for a generalized second order ODE
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

#if 0
   /**
    * Create an ABM4 state integrator for a generalized second order ODE
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
#endif


   /**
    * ABM4 uses two steps per cycle once primed.
    * @return Always returns 2.
    */
   virtual unsigned int get_transition_table_size (void) const
   { return 2; }

   /**
    * ABM4 needs a history of four derivatives.
    * @return Always returns 4.
    */
   virtual unsigned int get_history_length (void) const
   { return 4; }
};

}


#ifdef ER7_UTILS_NEED_AUX_INCLUDES
#include "er7_utils/integration/core/include/priming_integration_controls.hh"
#include "abm4_first_order_ode_integrator.hh"
#include "abm4_second_order_ode_integrator.hh"
#endif


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
