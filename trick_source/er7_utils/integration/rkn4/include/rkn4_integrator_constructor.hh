/*******************************************************************************

Purpose:
  (Define the class RKNystrom4IntegratorConstructor, which constructs
   integrators that use fourth order Runge Kutta Nystrom.)

Library dependencies:
  ((rkn4_integrator_constructor.o))

Programmers:
  (((David Hammen) (OSR) (July 2008) (JEOD_2.0) (Initial version))
   ((David Hammen) (OSR) (March 2009) (JEOD_2.0 #22) (Cleanup))
   ((David Hammen) (OSR) (Sept 2011) (JEOD 2.2 #537)
    (Multi-cycle support)))

*******************************************************************************/


#ifndef ER7_UTILS_RKNYSTROM4_INTEGRATOR_CONSTRUCTOR_HH
#define ER7_UTILS_RKNYSTROM4_INTEGRATOR_CONSTRUCTOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/integrator_constructor.hh"


namespace er7_utils {

/**
 * Create state and time integrators that propagate using standard RK4.
 */
class RKNystrom4IntegratorConstructor : public IntegratorConstructor {

ER7_UTILS_MAKE_SIM_INTERFACES(RKNystrom4IntegratorConstructor)

   // Note: The default constructor, copy constructor, assignment operator,
   // and destructor for this class are not declared.
   // The implicit C++ defaults suffice for this class.

public:
   // Static member functions.

   /**
    * Named constructor; create an RKNystrom4IntegratorConstructor instance.
    * The caller is responsible for deleting the returned object.
    * @return Newly created RKNystrom4IntegratorConstructor instance.
    */
   static IntegratorConstructor* create_constructor (void);


   // Member functions.

   /**
    * Return the class name.
    */
   virtual const char * get_class_name (void) const
   { return "RKNystrom4IntegratorConstructor"; }

   /**
    * Runge Kutta Nystrom 4 is a second order technique; it provides but does
    * not implement a first order ODE integrator. It also provides but does not
    * implement a generalized derivative second order ODE integrator.
    */
   virtual bool implements (Integration::ODEProblemType problem_type) const
   {
      return (problem_type != Integration::FirstOrderODE) &&
             (problem_type != Integration::GeneralizedDerivSecondOrderODE);
   }

   /**
    * Create a duplicate of the constructor.
    * The caller is responsible for deleting the returned object.
    * @return Duplicated constructor.
    */
   virtual IntegratorConstructor * create_copy (void) const;

   /**
    * Create an integration controls that guides the RKN4 integration process.
    * The caller is responsible for deleting the created object.
    * @return Integration controls object
    */
   virtual IntegrationControls * create_integration_controls (void) const;

   /**
    * Create an RK4 (not RKN4) state integrator for a first order ODE.
    * The caller is responsible for deleting the created object.
    * @return State integrator
    * @param[in]     size      State size
    * @param[in,out] controls  Integration controls
    */
   virtual FirstOrderODEIntegrator * create_first_order_ode_integrator (
      unsigned int size,
      IntegrationControls & controls) const;

   /**
    * Create an RKN4 state integrator for a simple second order ODE.
    * The caller is responsible for deleting the created object.
    * @return State integrator
    * @param[in]     size      State size
    * @param[in,out] controls  Integration controls
    */
   virtual SecondOrderODEIntegrator * create_second_order_ode_integrator (
      unsigned int size,
      IntegrationControls & controls) const;

   /**
    * Create an RK4 (not RKN4) state integrator for a generalized second order
    * ODE where generalized position is advanced with the use of the
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
    * Create an RKN4 state integrator for a generalized second order ODE
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
    * Indicate whether the integration technique explicitly solves a second
    * order ODE. Fourth order Runge Kutta Nystrom is such a technique.
    * @return Always returns true.
    */
   virtual bool is_second_order_ode_technique (void) const
   { return true; }

   /**
    * Fourth order Runge Kutta Nystrom uses four steps per cycle.
    * @return Always returns 4.
    */
   virtual unsigned int get_transition_table_size (void) const
   { return 4; }
};

}


#ifdef ER7_UTILS_NEED_AUX_INCLUDES
#include "er7_utils/integration/core/include/single_cycle_integration_controls.hh"
#include "er7_utils/integration/rk4/include/rk4_first_order_ode_integrator.hh"
#include "er7_utils/integration/rk4/include/rk4_second_order_ode_integrator.hh"
#include "rkn4_second_order_ode_integrator.hh"
#endif


#endif
