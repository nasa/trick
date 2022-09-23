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
 * Defines the methods for class IntegrationControls.
 */

/*
Purpose: ()
*/

// System includes
#include <cstddef>

// Interface includes
#include "er7_utils/interface/include/alloc.hh"
#include "er7_utils/interface/include/message_handler.hh"

// Local includes
#include "../include/bogus_integration_controls.hh"
#include "../include/integration_messages.hh"



namespace er7_utils {


// Die. This class exists for one purpose, which is to die when put to use.
unsigned int
BogusIntegrationControls::integrate (
   double,
   double,
   TimeInterface&,
   IntegratorInterface&,
   BaseIntegrationGroup&)
{
   MessageHandler::fail (
      __FILE__, __LINE__,
      IntegrationMessages::internal_error,
      "Integration group has not been initialized.");

      return 0;
}


// Clone a BogusIntegrationControls.
BogusIntegrationControls *
BogusIntegrationControls::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
