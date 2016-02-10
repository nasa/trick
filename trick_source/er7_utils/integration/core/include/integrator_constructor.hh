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
 * Defines the class IntegratorConstructor, the base class used to construct
 * synchronized integration controls and state integrators.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_INTEGRATOR_CONSTRUCTOR_HH
#define ER7_UTILS_INTEGRATOR_CONSTRUCTOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"
#include "er7_utils/interface/include/deletable.hh"

// Integrator includes
#include "generalized_position_derivative.hh"
#include "integration_technique.hh"


namespace er7_utils {

class IntegrationControls;
class IntegratorResult;
class IntegratorResultMerger;
class FirstOrderODEIntegrator;
class SecondOrderODEIntegrator;

/**
 * The base class for all integrator constructors.
 * An integrator constructor creates time and state integrators that work in
 * a synchronized fashion. Consider the midpoint method. A midpoint method
 * state integrator propagates state to the midpoint of the integration time
 * interval on the first intermediate step of an integration cycle. It then
 * propagates state to the end of the integration time interval on the second
 * (and last) intermediate step. The state integrator dictates the behavior
 * of the midpoint method time integrator.

 * Some other technique will involve a different number of intermediate steps
 * that jump to different points in time. The idea of an integrator constructor
 * is to ensure that the method used for propagating time is consistent with
 * the method used to propagate state.
 */
class IntegratorConstructor : virtual public Er7UtilsDeletable {
ER7_UTILS_MAKE_SIM_INTERFACES(IntegratorConstructor)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is protected / does nothing.


public:

   /**
    * IntegratorConstructor destructor.
    *
    * Note well: This class and its derived classes do allocate resources,
    * in the form of the created state and time integrators. However,
    * the burden of deleting those integrators falls upon the caller
    * rather than on this or derivative classes.
    */
   virtual ~IntegratorConstructor (void) {}


   /**
    * Identify the integrator constructor by class name.
    * Some of the constructors cannot create state integrators for all of
    * the create_XXX_integrator methods elaborated below.
    * The default implementation is to croak, identifying the technique by name.
    */
   virtual const char * get_class_name (void) const = 0;


   /**
    * Indicates whether the integrator constructor implements the
    * create_XXX_integrator function corresponding to the problem type:
    *
    * Problem Type                   | create_XXX_integrator Function
    * -------------------------------|-----------------------------------
    * FirstOrderODE                  | first_order_ode
    * SimpleSecondOrderODE           | second_order_ode
    * GeneralizedDerivSecondOrderODE | generalized_deriv_second_order_ode
    * GeneralizedStepSecondOrderODE  | generalized_step_second_order_ode
    *
    * Note that the default is to always return true. Techniques that do
    * not implement all of the create_XXX_integrator functions should
    * override this default implementation.
    *
    * @param problem_type  Specifies the type of ODE to be solved.
    * @return True if the constructor provides a means to solve the problem,
    *         false otherwise.
    *
    */
   virtual bool provides (
      Integration::ODEProblemType problem_type ER7_UTILS_UNUSED)
   const
   { return true; }


   /**
    * Indicates whether the problem type is implemented using the
    * integration technique corresponding to the integrator constructor.
    * This is distinct from provides() in that an integrator constructor
    * can implement the corresponding create_XXX_integrator function by
    * creating a state integrator from a different but related technique.
    *
    * Example: Symplectic Euler, being a second order technique, cannot solve
    * first order ODE problems. The symplectic Euler integrator constructor
    * implements its create_first_order_ode_integrator function by creating
    * a first order Euler state integrator to act as a surrogate.
    * Thus symplectic Euler returns true for provides(FirstOrderODE) but
    * returns false for implements(FirstOrderODE).
    *
    * Note that the default is to always return true. Techniques that do
    * not implement all of the create_XXX_integrator functions should
    * override this default implementation.
    *
    * @param problem_type  Specifies the type of ODE to be solved.
    * @return True if the technique directly implements a solution to the
    *         specified problem, false otherwise.
    *
    */
   virtual bool implements (
      Integration::ODEProblemType problem_type ER7_UTILS_UNUSED)
   const
   { return true; }


   /**
    * There's no need to call the typically computationally expensive derivative
    * on a step if the derivatives aren't used in that step. Some of the
    * implemented techniques do not use the results from these derivative jobs
    * on the initial step of an integration cycle. This method indicates whether
    * the integration technique needs the derivative jobs called on the initial
    * step of an integration cycle.
    * The default is to return true; all but a small number of the implemented
    * techniques do use the derivative information on the initial step. Those
    * techniques that don't make use of the derivative calculations should
    * override this method.
    * @return True if the technique needs derivatives on the initial step.
    */
   virtual bool need_first_step_derivatives (void) const
   { return true; }


   /**
    * Indicate whether the integration technique will always takes a
    * pre-determined number of integration steps per integration cycle.
    * A fixed-step integrator may not always use the same number of steps
    * every cycle. For example, a technique that requires priming may use
    * a different number of steps during priming than it uses once primed.
    * To qualify as a fixed-step integrator,
    *  - The number of steps must be determinable at any point in the cycle, and
    *  - The maximum number of steps must be fixed.@n
    * The default implementation is to return true; the technique is fixed-step.
    * Variable step integrators should override this default.
    * @return True if the technique is a fixed step integrator.
    */
   virtual bool is_fixed_step_integrator (void) const
   { return true; }


   /**
    * Indicate whether the integration technique explicitly solves a second
    * order ODE.
    *
    * The default implementation is to return false. Second order techniques
    * should override this default.
    *
    * @return True if the technique is for a second order ODE,
    *    false if the technique targets first order ODEs.
    */
   virtual bool is_second_order_ode_technique (void) const
   { return false; }


   /**
    * Return the buffer size needed by engines such as Trick that need to
    * allocate space separately from the integration technique.@n
    * The default implementation is to return the transition table size.
    * @return Largest number of steps in any integration cycle
    */
   virtual unsigned int get_buffer_size (void) const
   { return get_transition_table_size(); }


   /**
    * Return the size of the state transition table.
    * This pertains to the table needed by the technique itself,
    * not techniques used for priming.
    * @return Number of states in the state machine.
    */
   virtual unsigned int get_transition_table_size (void) const = 0;


   /**
    * Create a copy of 'this', a derived IntegratorConstructor object.
    * @return Clone of 'this'.
    */
   virtual IntegratorConstructor * create_copy (void) const = 0;


   /**
    * Create an integration controls object that guides the integration process.
    * @return Constructed integration controls object
    */
   virtual IntegrationControls * create_integration_controls (
      void) const = 0;


   /**
    * Create an integrator results merger object that merges the results from
    * a single integrator into an overall result.
    * @return Constructed integrator results merger object.
    */
   virtual IntegratorResultMerger * create_integrator_results_merger (
      void) const;


   /**
    * Create an integrator that propagates a first order ODE.
    *
    * Note well: The default implementation complains and returns NULL.
    * A derived class should override this default implementation and
    * create a FirstOrderODEIntegrator that implements the technique
    * corresponding to that derived class.
    *
    * @return Constructed state integrator
    * @param[in]      size     State size
    * @param [in,out] controls Integration controls to associate with the
    *    created integrator.
    */
   virtual FirstOrderODEIntegrator * create_first_order_ode_integrator (
      unsigned int size,
      IntegrationControls & controls) const;


   /**
    * Create an integrator that propagates a second order ODE for which the
    * time derivative of position is the velocity.
    *
    * Note well: The default implementation complains and returns NULL.
    * A derived class should override this default implementation and
    * create a SecondOrderODEIntegrator that implements the technique
    * corresponding to that derived class.
    *
    * @return Constructed state integrator
    * @param[in]      size     State size
    * @param [in,out] controls Integration controls to associate with the
    *    created integrator.
    */
   virtual SecondOrderODEIntegrator * create_second_order_ode_integrator (
      unsigned int size,
      IntegrationControls & controls) const;


   /**
    * Create an integrator that propagates a second order ODE for which the
    * time derivative of generalized position is a function of generalized
    * position and generalized velocity.
    *
    * Note well: The default implementation complains and returns NULL.
    * A derived class should override this default implementation and
    * create a SecondOrderODEIntegrator that implements the technique
    * corresponding to that derived class.
    *
    * @return Constructed state integrator
    * @param[in]     position_size  Size of the generalized position vector
    * @param[in]     velocity_size  Size of the generalized velocity vector
    * @param[in]     deriv_funs     Derivative functions container
    * @param[in,out] controls       Integration controls to associate with the
    *    created integrator.
    */
   virtual SecondOrderODEIntegrator *
   create_generalized_deriv_second_order_ode_integrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls) const;


   /**
    * Create an integrator that propagates a second order ODE in which the
    * generalized position is propagated via the position step function.
    * Note well: The default implementation complains and returns NULL.
    * @return Constructed state integrator
    * @param[in]     position_size  Size of the generalized position vector
    * @param[in]     velocity_size  Size of the generalized velocity vector
    * @param[in]     step_funs      Step functions container
    * @param[in,out] controls       Integration controls to associate with the
    *    created integrator.
    */
   virtual SecondOrderODEIntegrator *
   create_generalized_step_second_order_ode_integrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls) const;


protected:

   /**
    * IntegratorConstructor default constructor.
    */
   IntegratorConstructor (void)
   : Er7UtilsDeletable()
   { }

   /**
    * IntegratorConstructor copy constructor.
    */
   IntegratorConstructor (const IntegratorConstructor &)
   : Er7UtilsDeletable()
   { }

   /**
    * IntegratorConstructor assignment operator; no-op.
    */
   IntegratorConstructor & operator= (const IntegratorConstructor &)
   { return *this; }

};

}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
