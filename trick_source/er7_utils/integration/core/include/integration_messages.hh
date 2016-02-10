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
 * Defines the class IntegrationMessages, the class that specifies the
 * message IDs used in the integration model.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_INTEGRATION_MESSAGES_HH
#define ER7_UTILS_INTEGRATION_MESSAGES_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"


namespace er7_utils {

/**
 * Declares messages associated with the integration model.
 */
class IntegrationMessages {

   // This is a static class.
   // The default constructor, copy constructor, destructor, and assignment
   // operator for this class are private / unimplemented.

public:

   // Static member data

   static char const * unsupported_option; /**< trick_units(--) @n
      Issued when some user input is invalid. */

   static char const * invalid_item; /**< trick_units(--) @n
      Issued when an item is somehow invalid; a duplicate entry for example. */

   static char const * internal_error; /**< trick_units(--) @n
      Issued when the ER7_UTILS programmer messed up. */

   static char const * invalid_request; /**< trick_units(--) @n
      Issued when a non-ER7_UTILS programmer messed up. */

   static char const * information; /**< trick_units(--) @n
      Issued in non-error messages. */


private:

   /**
    * Not implemented.
    */
   IntegrationMessages (void);

   /**
    * Not implemented.
    */
   IntegrationMessages (const IntegrationMessages &);

   /**
    * Not implemented.
    */
   ~IntegrationMessages (void);

   /**
    * Not implemented.
    */
   IntegrationMessages & operator= (const IntegrationMessages &);

};

}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
