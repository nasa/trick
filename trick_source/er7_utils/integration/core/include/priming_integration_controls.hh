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
 * Defines the class PrimingIntegrationControls, the class used for controlling
 * the ABM4 and Beeman integration processes.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_PRIMING_INTEGRATION_CONTROLS_HH
#define ER7_UTILS_PRIMING_INTEGRATION_CONTROLS_HH

// System includes
#include <cstddef>

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Model includes
#include "standard_integration_controls.hh"


namespace er7_utils {

class IntegratorConstructor;

/**
 * A priming integration controls object provides mechanisms for controlling
 * integrators that need a simple priming process.
 */
class PrimingIntegrationControls : public StandardIntegrationControls {
ER7_UTILS_MAKE_SIM_INTERFACES(PrimingIntegrationControls)

public:

   // Constructors and destructor.

   /**
    * PrimingIntegrationControls default constructor.
    */
   PrimingIntegrationControls (void)
   :
      Er7UtilsDeletable (),
      StandardIntegrationControls (),
      priming_controls (NULL),
      priming_count (0),
      history_length (0)
   { }


   /**
    * PrimingIntegrationControls copy constructor.
    * @param[in] src  Object to be copied.
    * Note that this is a deep rather than shallow copy.
    */
   PrimingIntegrationControls (
      const PrimingIntegrationControls & src)
   :
      Er7UtilsDeletable (),
      StandardIntegrationControls (src),
      priming_controls (src.priming_controls ?
                        src.priming_controls->create_copy() : NULL),
      priming_count (src.priming_count),
      history_length (src.history_length)
   {
      if (priming_controls != NULL) {
         set_alt_controls (*priming_controls);
      }
   }


   /**
    * PrimingIntegrationControls non-default constructor.
    * @param[in] primer_constructor       Constructor that creates the primer
    * @param[in] history_buffer_size      History buffer size
    * @param[in] number_operating_stages  Number operational stages
    */
   PrimingIntegrationControls (
      const IntegratorConstructor & primer_constructor,
      unsigned int history_buffer_size,
      unsigned int number_operating_stages);


   /**
    * PrimingIntegrationControls destructor.
    */
   virtual ~PrimingIntegrationControls (void);


   /**
    * PrimingIntegrationControls assignment operator.
    * @param[in] src  Object to be copied.
    */
   PrimingIntegrationControls & operator= (PrimingIntegrationControls src)
   {
      swap (src);
      return *this;
   }


   // Additional member functions

   /**
    * Get the integration controls used during priming.
    */
   IntegrationControls & get_priming_controls ()
   { return *priming_controls; }

   /**
    * Reset the integration controls.
    */
   virtual void reset_integrator (void);

   /**
    * Prepare for a new integration tour.
    */
   virtual void start_integration_tour (void);

   /**
    * Create a copy of 'this' PrimingIntegrationControls object.
    * @return Clone of 'this'.
    */
   virtual PrimingIntegrationControls * create_copy () const;


protected:

   // Member functions.

   /**
    * Non-throwing swap function.
    * Swap contents of 'this' with that of another PrimingIntegrationControls.
    * @param[in] other  Item with which contents are to be swapped.
    */
   virtual void swap (PrimingIntegrationControls & other);

   using StandardIntegrationControls::swap;


   // Member data.

   IntegrationControls * priming_controls; /**< trick_units(--) @n
      The integration controls object used to prime the state integrators. */

   int priming_count; /**< trick_units(--) @n
      The number of priming integration cycles the integrator to go.
      This is typically reset to history_length-1. */

   unsigned int history_length; /**< trick_units(--) @n
      The number of items in the state integrator's history buffer. */
};

}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
