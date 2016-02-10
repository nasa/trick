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
 * Defines the class StandardIntegrationControls, the class that defines
 * the most generic integration controls.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_STANDARD_INTEGRATION_CONTROLS_HH
#define ER7_UTILS_STANDARD_INTEGRATION_CONTROLS_HH

// System includes
#include <cstddef>

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Local includes
#include "integration_controls.hh"


namespace er7_utils {

// Forward declarations
class TimeInterface;
class BaseIntegrationGroup;


/**
 * The class StandardIntegrationControls extends the base IntegrationControls
 * class for general purpose operations. Compare with the class
 * SingleCycleIntegrationControls, which extends IntegrationControls for
 * integration techniques that always use but one integration tour comprising
 * but one integration cycle to complete the integration process.
 */
class StandardIntegrationControls : public IntegrationControls {
ER7_UTILS_MAKE_SIM_INTERFACES(StandardIntegrationControls)

public:

   // Constructors and destructor.

   /**
    * StandardIntegrationControls default constructor.
    */
   StandardIntegrationControls (void)
   :
      Er7UtilsDeletable (),
      IntegrationControls (),
      cycle_starttime (0.0),
      cycle_simdt (0.0),
      cycle_dyndt (0.0),
      multi_cycle (false),
      /* target_attained (true), */
      alt_controls (NULL)
   {}


   /**
    * StandardIntegrationControls non-default constructor.
    * @param[in] number_stages_in  Number of stages in an integration cycle.
    */
   explicit StandardIntegrationControls (unsigned int number_stages_in)
   :
      Er7UtilsDeletable (),
      IntegrationControls (number_stages_in),
      cycle_starttime (0.0),
      cycle_simdt (0.0),
      cycle_dyndt (0.0),
      multi_cycle (false),
      /* target_attained (true), */
      alt_controls (NULL)
   {}


   /**
    * StandardIntegrationControls copy constructor.
    * @param[in] src  Object to be copied.
    * Note that the alt_controls is not copied.
    * Properly setting this member is the responsibility of a derived class'
    * copy constructor.
    */
   StandardIntegrationControls (const StandardIntegrationControls & src)
   :
      Er7UtilsDeletable (),
      IntegrationControls (src),
      cycle_starttime (src.cycle_starttime),
      cycle_simdt (src.cycle_simdt),
      cycle_dyndt (src.cycle_dyndt),
      multi_cycle (src.multi_cycle),
      /* target_attained (src.target_attained), */
      alt_controls (NULL)
   {}


   /**
    * StandardIntegrationControls destructor.
    */
   virtual ~StandardIntegrationControls (void)
   {}


   // Member functions.

   /**
    * StandardIntegrationControls assignment operator.
    */
   StandardIntegrationControls & operator= (StandardIntegrationControls src)
   {
      swap (src);
      return *this;
   }


   /**
    * Create a copy of 'this' StandardIntegrationControls object.
    * @return Clone of 'this'.
    */
   virtual StandardIntegrationControls * create_copy () const;


   /**
    * Prepare for the upcoming cycle of integrations.
    * The integrate function calls this function at the start of a tour.
    *
    * The default implementation sets integ_starttime and cycle_starttime to the
    * provided starttime, integ_simdt and cycle_simdt to the provided simdt.
    * Multi-cycle and multi-step integrators that override this default
    * implementation should set integ_starttime and integ_simdt set to the
    * provided inputs, and should also set
    *  - cycle_starttime and cycle_simdt based on the corresponding integ
    *    values and the state of the technique-specific integration control.
    * The overrides are free to do many do other things such as switching
    * transition tables, etc.
    */
   virtual void start_integration_tour (void)
   {
      cycle_starttime  = integ_starttime;
      cycle_simdt      = integ_simdt;
      cycle_dyndt      = integ_dyndt;
      integ_simtime    = integ_starttime;
      integ_time_scale = 0.0;
   }


   /**
    * Indicate whether this is the end of a tour.
    *
    * The default implementation always returns true.
    * Integrators that override this method must return true at the end
    * of a tour, false otherwise. Overrides can also perform any
    * technique-specific actions that need to be taken at the end of a cycle.
    */
   virtual bool end_integration_cycle (void)
   {
      return true;
   }


   /**
    * Perform one step of the integration process.
    * @return Step number; a value of zero means the integration process
    *    has been completed.
    * @param[in] start_time
    *    The simulation engine time from which the integrators start.
    *    The integration process must eventually reach starttime+sim_dt.
    *    This function should return zero when the integration tour has finally
    *    reached this tour end time, non-zero until then.
    * @param[in] sim_dt
    *    The difference between the simulation time at the end and start of the
    *    integration tour.
    * @param[in,out] time_if
    *    Object external to the ER7 utilities suite that represents time.
    * @param[in,out] integ_interface
    *    Interface with the simulation engine for this integration controls.
    * @param[in,out] integ_group
    *    The integration group that contains this integration controls.
    *
    * Note well: The starttime and sim_dt are assumed to be constant during
    * an integration tour.
    */
   virtual unsigned int integrate (
      double start_time, double sim_dt,
      TimeInterface & time_if,
      IntegratorInterface & integ_interface,
      BaseIntegrationGroup & integ_group);


protected:

   // Member functions.

   /**
    * Set the alternative controller to the provided value.
    * @param[in] alt_controller  New alternate controller
    */
   void set_alt_controls (IntegrationControls & alt_controller)
   {
      alt_controls = &alt_controller;
      multi_cycle = true;
   }


   /**
    * Clear the alternative controller.
    * @param[in] is_multi_cycle  New value for multi_cycle
    */
   void clear_alt_controls (bool is_multi_cycle = false)
   {
      alt_controls = NULL;
      multi_cycle = is_multi_cycle;
   }


   /**
    * Non-throwing swap function.
    * Swap contents of 'this' with that of another StandardIntegrationControls.
    * @param[in] other  Item with which contents are to be swapped.
    */
   virtual void swap (StandardIntegrationControls & other);

   using IntegrationControls::swap;


   // Member data

   double cycle_starttime; /**< trick_units(--) @n
      The simulation engine time of the start of the current integration cycle
      as determined by start_integration_tour or end_integration_cycle.
      For single cycle integrators such as RK4, the cycle_starttime and
      cycle_simdt are equal to the integ_starttime and integ_simdt.
      Multi-cycle integrators such as Gauss-Jackson compute the
      cycle_starttime and cycle_simdt. */

   double cycle_simdt; /**< trick_units(--) @n
      The simulation engine time span of the current integration cycle
      as determined by start_integration_tour or end_integration_cycle. */

   double cycle_dyndt; /**< trick_units(--) @n
      The JEOD dynamic time span corresponding to cycle_simdt. */


   bool multi_cycle; /**< trick_units(--) @n
      Always false for single-cycle integrators.
      Multi-cycle integrators must set this to true to operate in
      multi-cycle mode. */

#if 0
   bool target_attained; /**< trick_units(--) @n
      True if an state integrators attained the target_stage,
      false otherwise. */
#endif

private:

   // Member data

   IntegrationControls * alt_controls; /**< trick_units(--) @n
      An alternate controls, typically for priming, that acts as a stand-in
      for this integration controller. */
};

}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
