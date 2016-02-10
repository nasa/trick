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
 * Implements the class IntegrationMessages.
 */

/*
Purpose: ()
*/


// System includes

// Local includes
#include "../include/integration_messages.hh"


/**
 * Make a message code in class IntegrationMessages.
 */
#define MAKE_MESSAGE_CODE(id) \
    char const * IntegrationMessages::id = "er7_utils/integration/" #id


namespace er7_utils {

MAKE_MESSAGE_CODE(unsupported_option);
MAKE_MESSAGE_CODE(invalid_item);
MAKE_MESSAGE_CODE(internal_error);
MAKE_MESSAGE_CODE(invalid_request);
MAKE_MESSAGE_CODE(information);

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
