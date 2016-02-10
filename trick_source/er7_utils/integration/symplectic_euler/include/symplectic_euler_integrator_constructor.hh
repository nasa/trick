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
 * Defines the class SymplecticEulerIntegratorConstructor, which constructs
 * state integrators that use symplectic Euler integration.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_SYMPLECTIC_EULER_INTEGRATOR_CONSTRUCTOR_HH
#define ER7_UTILS_SYMPLECTIC_EULER_INTEGRATOR_CONSTRUCTOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/integrator_constructor.hh"


namespace er7_utils {


/**
 * Create controls and integrators and that propagate using symplectic Euler.
 */
class SymplecticEulerIntegratorConstructor : public IntegratorConstructor {

ER7_UTILS_MAKE_SIM_INTERFACES(SymplecticEulerIntegratorConstructor)

public:

   // Static member functions.

   /**
    * Named constructor; create a SymplecticEulerIntegratorConstructor.
    * The caller is responsible for deleting the returned object.
    * @return Newly created SymplecticEulerIntegratorConstructor.
    */
   static IntegratorConstructor* create_constructor (void);


   // Constructors and destructor.

   // Note: The copy constructor and assignment operator for this
   // class are not declared. The C++ defaults suffice for this class.

   /**
    * SymplecticEulerIntegratorConstructor default constructor.
    */
   SymplecticEulerIntegratorConstructor (void)
   { }

   /**
    * SymplecticEulerIntegratorConstructor destructor.
    */
   virtual ~SymplecticEulerIntegratorConstructor (void)
   { }


   // Member functions.

   /**
    * Return the class name.
    */
   virtual const char * get_class_name (void) const
   { return "SymplecticEulerIntegratorConstructor"; }

   /**
    * Symplectic Euler is a second order technique; it provides but does not
    * implement a first order ODE integrator.
    */
   virtual bool implements (Integration::ODEProblemType problem_type) const
   {
      return problem_type != Integration::FirstOrderODE;
   }

   /**
    * Create a duplicate of the constructor.
    * The caller is responsible for deleting the returned object.
    * @return Duplicated constructor.
    */
   virtual IntegratorConstructor * create_copy (void) const;

   /**
    * Create an integration controls that guides the symplectic Euler
    * integration process.
    * The caller is responsible for deleting the created object.
    * @return Integration controls object
    */
   virtual IntegrationControls * create_integration_controls (void) const;

   /**
    * Create an Euler state integrator for a first order ODE.
    * Note that this creates an Euler integrator as a surrogate.
    * There is no such thing as a first order symplectic Euler integrator.
    * The caller is responsible for deleting the created object.
    * @return State integrator
    * @param[in]     size      State size
    * @param[in,out] controls  Integration controls
    */
   virtual FirstOrderODEIntegrator * create_first_order_ode_integrator (
      unsigned int size,
      IntegrationControls & controls) const;

   /**
    * Create a symplectic Euler state integrator for a simple second order ODE.
    * The caller is responsible for deleting the created object.
    * @return State integrator
    * @param[in]     size      State size
    * @param[in,out] controls  Integration controls
    */
   virtual SecondOrderODEIntegrator * create_second_order_ode_integrator (
      unsigned int size,
      IntegrationControls & controls) const;

   /**
    * Create a symplectic Euler state integrator for a generalized second order
    * ODE where generalized position is advanced with the use of the derivatives
    * function.
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
    * Create a symplectic Euler state integrator for a generalized second order
    * ODE where generalized position is advanced with the use of the step
    * function.
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
    * Indicate whether the integration technique explicitly solves a second
    * order ODE. Symplectic Euler is such a technique.
    * @return Always returns true.
    */
   virtual bool is_second_order_ode_technique (void) const
   { return true; }


   /**
    * Euler uses one step per cycle.
    * @return Always returns 1.
    */
   virtual unsigned int get_transition_table_size (void) const
   { return 1; }
};

}


#ifdef ER7_UTILS_NEED_AUX_INCLUDES
#include "er7_utils/integration/core/include/single_cycle_integration_controls.hh"
#include "er7_utils/integration/euler/include/euler_first_order_ode_integrator.hh"
#include "symplectic_euler_second_order_ode_integrator.hh"
#endif


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
