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
 * Defines non-inline functions of er7_utils::alloc.
 */

/*
Purpose: ()
*/


// System includes
#include <cstddef>
#include <cstdlib>

// Local includes
#include "../include/alloc.hh"


#ifdef ER7_UTILS_HAVE_ABI
#include <cxxabi.h>
#endif


namespace er7_utils {

namespace alloc {

#ifdef ER7_UTILS_HAVE_ABI

// Demangle a type ID.
const std::string
demangle (
   const std::type_info & info)
{
   std::string result;
   bool have_result = false;

   const char * type_name = NULL;
   int status;

   type_name = abi::__cxa_demangle (info.name(), NULL, NULL, &status);

   if (status == 0) {
      result = type_name;
      have_result = true;
   }
   else {
      have_result = false;
   }

   if (type_name != NULL) {
      std::free (const_cast<char *>(type_name));
      type_name = NULL;
   }

   // Punted by the above: Use the mangled name.
   if (! have_result) {
      result = info.name();
   }

   return result;
}

#endif



}

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */
