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
 * Defines the extensible class BaseIntegrationGroup, an instance of which is
 * responsible for integrating the states of a set of DynBody objects.
 */

/*
Purpose: ()
*/

#ifndef ER7_UTILS_BASE_INTEGRATION_GROUP_HH
#define ER7_UTILS_BASE_INTEGRATION_GROUP_HH

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Local includes
#include "integration_controls.hh"
// #include "integrator_interface.hh"
#include "integrator_result.hh"


namespace er7_utils {

// Forward declarations
class IntegratorConstructor;
class IntegratorInterface;
class TimeInterface;

/**
 * A BaseIntegrationGroup integrates the state of a set of objects over time.
 * This class is designed for extensibility. Authors of derived classes should
 * follow the extension notes in the source file.
 *
 * Note that an IntegrableObject is not an Er7UtilsDeletable.
 */
class BaseIntegrationGroup {

ER7_UTILS_MAKE_SIM_INTERFACES(BaseIntegrationGroup)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * BaseIntegrationGroup destructor.
    */
   virtual ~BaseIntegrationGroup ();


   /**
    * Reset the group's integrators.
    *
    * Resets can occur when time changes behavior (call is internal to the
    * integration process) or when some external event would render an
    * integrator's history invalid (call to reset comes from outside).
    * When either happens, integrators that depend on history need to reset
    * their internal state to indicate that the saved data are invalid.
    */
   void reset_integrators ()
   {
      // Somehow reset the group's state integrators.
      reset_body_integrators();

      // Reset the integration controls.
      integ_controls->reset_integrator ();

      // Reset the integrator interface so derivatives will be calculated.
      integ_interface->reset_first_step_derivs_flag ();
   }


   /**
    * Get the integration interface's first_step_derivs_flag.
    */
   bool get_first_step_derivs_flag ()
   {
      return integ_interface->get_first_step_derivs_flag();
   }


   /**
    * Integrate to the next stage of the integration process.
    * This is a simple pass-through to the integration controls object.
    * @param sim_begtime Start time of integration cycle, in sim engine seconds
    * @param sim_deltime Duration of integration cycle, in sim engine seconds
    * @return            Integrator step number, zero = done
    */
   int integrate_group (
      double sim_begtime,
      double sim_deltime)
   ER7_UTILS_ALWAYS_INLINE
   {
      return integ_controls->integrate (
                sim_begtime,
                sim_deltime,
                *time_interface,
                *integ_interface,
                *this);
   }


   /**
    * Integrate to the next stage of the integration process.
    * This is a simple pass-through to the integration controls object.
    * @param sim_begtime Start time of integration cycle, in sim engine seconds
    * @return            Integrator step number, zero = done
    */
   int integrate_group_from (double sim_begtime)
   ER7_UTILS_ALWAYS_INLINE
   {
      return integrate_group (sim_begtime, integ_interface->get_dt());
   }


   /**
    * Integrate to the next stage of the integration process.
    * This is a simple pass-through to the integration controls object.
    * @param sim_endtime End time of integration cycle, in sim engine seconds
    * @return            Integrator step number, zero = done
    */
   int integrate_group_to (double sim_endtime)
   ER7_UTILS_ALWAYS_INLINE
   {
      double dt = integ_interface->get_dt();
      return integrate_group (sim_endtime-dt, dt);
   }


   // Virtual methods

   /**
    * Initialize the integration group.
    * Some integration techniques are configurable by user input, and thus
    * the creation of the controls and integrators needs to be delayed a bit.
    */
   virtual void initialize_group ();


   /**
    * Update the interface with the sim engine to reflect integration status.
    * @param[in] step_number  The integration step number
    */
   virtual void update_integration_interface (
      unsigned int step_number)
   {
      integ_interface->set_step_number (step_number);
      integ_interface->set_time (integ_controls->get_simtime());
   }


   // Pure virtual methods

   /**
    * Integrate the states of the integrable objects managed by this group.
    * This function should only be called by IntegrationControls::integrate
    * or by an override of that function.
    * @param[in]     cycle_dyndt   Dynamic time step, in dynamic time seconds.
    * @param[in]     target_stage  The stage of the integration process
    *                              that the integrator should try to attain.
    * @return The status (time advance, pass/fail status) of the integration.
    */
   virtual IntegratorResult integrate_bodies (
      double cycle_dyndt,
      unsigned int target_stage) = 0;


   /**
    * Reset the integrators for the integrable objects managed by this group.
    * Resets can occur when time changes behavior (call is internal to the
    * integration process) or when some external event would render an
    * integrator's history invalid (call comes from outside).
    * When either happens, integrators that depend on history need to reset
    * their internal state to indicate that the saved data are invalid.)
    */
   virtual void reset_body_integrators () = 0;


protected:

   // Constructors

   /**
    * BaseIntegrationGroup default constructor.
    * This constructor exists internally for checkpoint restart use
    * and to support derived classes' default constructors.
    */
   BaseIntegrationGroup ();

   /**
    * BaseIntegrationGroup non-default constructor.
    * @param[in] integ_cotr   Integrator constructor
    * @param[in] integ_inter  Integrator interface
    * @param[in] time_if      Time interface
    *
    * The integ_controls member is initialized to a BogusIntegrationControls
    * instance so that attempts to use a default-initialized group will fail
    * rather than crash.
    */
   BaseIntegrationGroup (
      IntegratorConstructor & integ_cotr,
      IntegratorInterface & integ_inter,
      TimeInterface & time_if);

   /**
    * BaseIntegrationGroup copy constructor.
    * @param[in] source       Object to be copied
    */
   BaseIntegrationGroup (
      const BaseIntegrationGroup & source);


   /**
    * Set the interfaces to the simulation engine and the time keeper.
    * @param[in] integ_inter  Integrator interface
    * @param[in] time_if      Time interface
    */
   void set_interfaces (
      IntegratorInterface & integ_inter,
      TimeInterface & time_if)
   {
      integ_interface = &integ_inter;
      time_interface = &time_if;
   }


   // Member functions

   /**
    * Non-throwing swap function.
    * Swap contents of 'this' with that of another BaseIntegrationGroup.
    * @param[in] other  Item with which contents are to be swapped.
    */
   virtual void swap (BaseIntegrationGroup & other);


   // Member data

   IntegratorConstructor * integ_constructor; /**< trick_units(--) @n
     The integrator constructor that is used to generate the integration
     controls for this object and is used to generate the state integrators for
     each integrated object whose state is integrated by this group. */

   IntegratorInterface * integ_interface; /**< trick_units(--) @n
     The interface between the integration module and the simulation engine's
     integration structure. */

   TimeInterface * time_interface; /**< trick_units(--) @n
     The interface between the integration module and the object that
     represents time. */

   IntegrationControls * integ_controls; /**< trick_units(--) @n
     The integration controls object that guides the integration process.
     This object is created at construction time by calling the
     integrator constructor's create_integration_controls method. */


private:

   /**
    * Not implemented.
    */
   BaseIntegrationGroup & operator= (const BaseIntegrationGroup &);
};


}

#ifdef ER7_UTILS_NEED_AUX_INCLUDES
#include "time_interface.hh"
#include "integrator_constructor.hh"
#include "integration_controls.hh"
#include "bogus_integration_controls.hh"
#endif


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
