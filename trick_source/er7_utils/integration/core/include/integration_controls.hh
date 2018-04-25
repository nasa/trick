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
 * Defines the class IntegrationControls, the base class used for controlling
 * the integration process. Every integration group contains an
 * IntegrationControls object, created by means of some integration constructor.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_INTEGRATION_CONTROLS_HH
#define ER7_UTILS_INTEGRATION_CONTROLS_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"
#include "er7_utils/interface/include/deletable.hh"

// Local includes
#include "integrator_interface.hh"


namespace er7_utils {

// Forward declarations
class TimeInterface;
class BaseIntegrationGroup;



/**
 * The class IntegrationControls defines the basics needed for controlling
 * the integration process.
 */
class IntegrationControls : virtual public Er7UtilsDeletable {

ER7_UTILS_MAKE_SIM_INTERFACES(IntegrationControls)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * IntegrationControls destructor.
    */
   virtual ~IntegrationControls (void);


   // Member functions.

   /**
    * Create a copy of 'this', a derived IntegrationControls object.
    * @return Clone of 'this'.
    */
   virtual IntegrationControls * create_copy (void) const = 0;


   // Getters for which this is no setter

   /**
    * Get the current integration time.
    * @return integ_simtime value.
    */
   double ER7_UTILS_ALWAYS_INLINE
   get_simtime (void) const
   {
      return integ_simtime;
   }

   /**
    * Get the final_stage data member.
    * @return final_stage value.
    */
   unsigned int ER7_UTILS_ALWAYS_INLINE
   get_final_stage (void) const
   {
      return final_stage;
   }

   /**
    * Get the step_number data member.
    * @return step_number value.
    */
   unsigned int ER7_UTILS_ALWAYS_INLINE
   get_step_number (void) const
   {
      return step_number;
   }

   /**
    * Determine whether the controller is in the initial cycle of a loop.
    * @return True if first cycle of a loop, false otherwise.
    */
   bool ER7_UTILS_ALWAYS_INLINE
   in_initial_cycle (void) const
   {
      return step_number == 0;
   }

   /**
    * Determine the expected step number, assuming integration succeeds.
    * @return Expected step number.
    */
   unsigned int ER7_UTILS_ALWAYS_INLINE
   get_expected_step_number (void) const
   {
      if (transition_table[cycle_stage] == final_stage) {
         return 0;
      }
      else {
         return step_number + 1;
      }
   }


   // Paired getters and setters

   /**
    * Get the reset_needed flag.
    * @return Value of reset_needed flag.
    */
   bool ER7_UTILS_ALWAYS_INLINE
   get_reset_needed (bool value ER7_UTILS_UNUSED) const
   {
      return reset_needed;
   }

   /**
    * Set the reset_needed flag.
    */
   void ER7_UTILS_ALWAYS_INLINE
   set_reset_needed ()
   {
      reset_needed = true;
   }

   /**
    * Get a transition table element.
    * @param[in] index  Table element to be retrieved.
    * @return  Transition table element value.
    */
   unsigned int
   get_transition_table_element (unsigned int index) const
   {
      return transition_table[index];
   }

   /**
    * Set a transition table element.
    * This method should be used with great caution.
    * @param[in] index  Table element to be set.
    * @param[in] value  Value to be inserted at that element.
    */
   void
   set_transition_table_element (
      unsigned int index,
      unsigned int value)
   {
      transition_table[index] = value;
   }


   // Integration methods

   /**
    * Make one step in the process that eventually integrates state from
    * the start_time to start_time+sim_dt.
    * @return Step number; zero when finished.
    * @param[in] start_time
    *    The simulation engine time at the start of the integration tour.
    * @param[in] sim_dt
    *    The difference between the simulation time at the end and start of the
    *    integration tour.
    * @param[in,out] time_interface
    *    Object external to the ER7 utilities suite that represents time.
    * @param[in,out] integ_interface
    *    Interface with the simulation engine for this integration controls.
    * @param[in,out] integ_group
    *    The integration group that contains this integration controls.
    */
   virtual unsigned int integrate (
      double start_time, double sim_dt,
      TimeInterface & time_interface,
      IntegratorInterface & integ_interface,
      BaseIntegrationGroup & integ_group) = 0;


   /**
    * Reset an integration controls for events such as changes in the
    * nature in time, discontinuities in the derivatives, etc.
    *
    * The default implementation does nothing.
    * Multi-cycle and multi-step integrators that override this default
    * implementation should respond in some technique-specific way to
    * that resets an integration controls to its initial state.
    */
   virtual void reset_integrator (void) {}


protected:

   // Constructors.

   /**
    * IntegrationControls default constructor.
    * This constructor is needed for restart.
    */
   IntegrationControls (void);

   /**
    * IntegrationControls copy constructor.
    * @param[in] source  The IntegrationControls to be copied.
    */
   IntegrationControls (const IntegrationControls & source);

   /**
    * IntegrationControls non-default constructor.
    * This is the constructor invoked by an integrator constructor.
    * @param[in] number_stages_in  Number of stages in an integration cycle.
    */
   explicit IntegrationControls (unsigned int number_stages_in);


   // Member functions.

   /**
    * Non-throwing swap function.
    * Swap contents of 'this' with that of another IntegrationControls.
    * @param[in] other  Item with which contents are to be swapped.
    */
   virtual void swap (IntegrationControls & other);


   // Member data.

   double time_scale_factor; /**< trick_units(--) @n
      Ratio of dynamic delta time to simulation engine delta time. */


   double integ_starttime; /**< trick_units(--) @n
      The simulation time of the start of the current integration loop as
      requested by the simulation engine. */

   double integ_simdt; /**< trick_units(--) @n
      The simulation time span of the current integration loop as
      requested by the simulation engine. */

   double integ_dyndt; /**< trick_units(--) @n
      The dynamic time span corresponding to integ_simdt. */

   double integ_time_scale; /**< trick_units(--) @n
      The time scale factor, with 0.0 representing the start of the
      integration interval and 1.0 representing the end of the interval. */

   double integ_simtime; /**< trick_units(--) @n
      The current simulation time. */


   unsigned int * transition_table; /**< trick_units(--) @n
      A number_stages array that directs the transitions from one integration
      stage to the next.
      The constructor for this class populates this array with values
      1 (element 0) to number_stages (element number_stages-1). */


   unsigned int step_number; /**< trick_units(--) @n
      Starts at zero at the start of a tour, increments by one per stage,
      and resets to zero at the end of a tour. */

   unsigned int cycle_stage; /**< trick_units(--) @n
      The technique-specific finite state machine state number of the current
      stage. The cycle_stage starts at zero, nominally incrementing by one for
      each cycle, until it attains a value equal to the final_stage. At this
      point the cycle begins anew with a cycle_stage of zero. */

#if 0
   unsigned int target_stage; /**< trick_units(--) @n
      The technique-specific finite state machine state number that the TwoState
      integrators should attempt to reach, nominally cycle_stage+1. Simple
      integrators will always reach this state. More complex ones may not, in
      which case they should set attained_stage to indicate the stage that was
      reached. */
#endif

   unsigned int final_stage; /**< trick_units(--) @n
      The stage number at which the cycle_stage resets to zero. */

   unsigned int number_stages; /**< trick_units(--) @n
      The size of the transition table, nominally final_stage. */

   bool reset_needed; /**< trick_units(--) @n
      Wen set, indicates that a reset needs to be performed
      at the start of the next integration cycle.
      The flag will be cleared when the reset is performed. */


private:

   /**
    * Not implemented.
    */
   IntegrationControls & operator= (const IntegrationControls &);
};

}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
