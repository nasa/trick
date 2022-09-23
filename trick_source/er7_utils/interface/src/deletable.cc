/**
 * @if Er7UtilsUseGroups
 * @addtogroup Er7Utils
 * @{
 * @addtogroup Interface
 * @{
 * @endif
 */

/**
 * @file
 * Defines Er7UtilsDeletable methods.
 */

/*
Purpose: ()
*/


// System includes
#include <cstddef>

// Interface includes
#include "er7_utils/interface/include/alloc.hh"

// Local includes
#include "../include/deletable.hh"


namespace er7_utils {

/**
 * Delete an instance
 * @param[in,out] instance  Object to be deleted.
 */
void
Er7UtilsDeletable::delete_instance_internal (
   Er7UtilsDeletable* instance)
{
   alloc::delete_object (instance);
}

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
