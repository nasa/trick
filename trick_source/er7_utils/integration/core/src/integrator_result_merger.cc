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
 * Defines member functions for the class IntegratorResultMerger.
 */

/*
Purpose: ()
*/

// Local includes
#include "../include/integrator_result_merger.hh"
#include "../include/integrator_result.hh"
#include "../include/integration_messages.hh"

// Interface includes
#include "er7_utils/interface/include/alloc.hh"
#include "er7_utils/interface/include/message_handler.hh"

// System includes
#include <cmath>


namespace er7_utils {

// Merge an IntegratorResult into another.
bool
IntegratorResultMerger::merge_integrator_result (
   const IntegratorResult & new_result,
   IntegratorResult & merged_result)
const
{
   bool success = true;

   // Leave the old result as-is if the new result is empty.
   if (new_result.get_merge_count() == 0) {
      ; // Empty
   }

   // Take the new result as-is if the merged result is currently empty.
   else if (merged_result.get_merge_count() == 0) {
      merged_result = new_result;
   }

   // A true merge is needed if something has already been merged in.
   else {

      // A new failure means the merged result failed.
      // Note that there is no test for a mismatch; in this simple
      // implementation failure modes are assumed to be zero or one only.
      if (new_result.get_failure_mode() != 0) {
         merged_result.set_failure_mode (new_result.get_failure_mode());
      }

      // Test for a time scale mismatch.
      if (std::abs (new_result.get_time_scale() -
                    merged_result.get_time_scale()) > 1e-15) {
         MessageHandler::error (
            __FILE__, __LINE__,
            IntegrationMessages::invalid_item,
            "Time scale mismatch between %.16f (new) and %.16f (old).",
            new_result.get_time_scale(),
            merged_result.get_time_scale());
         success = false;
      }

      // Merge the counts.
      merged_result.increment_merge_count (new_result.get_merge_count());
   }

   return success;
}


// Clone a IntegratorResultMerger object.
IntegratorResultMerger *
IntegratorResultMerger::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Croak when merging an IntegratorResult into another.
bool
BogusIntegratorResultMerger::merge_integrator_result (
   const IntegratorResult &,
   IntegratorResult &)
const
{
   MessageHandler::fail (
      __FILE__, __LINE__,
      IntegrationMessages::invalid_request,
      "Integration group has not been properly initialized.");

   return false;
}


// Clone a BogusIntegratorResultMerger object.
IntegratorResultMerger *
BogusIntegratorResultMerger::create_copy ()
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
