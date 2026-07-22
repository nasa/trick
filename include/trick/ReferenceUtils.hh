#ifndef REFERENCE_UTILS_HH
#define REFERENCE_UTILS_HH

/*
    PURPOSE: ( Provides helper functions for working with REF2. )
*/

#include "trick/reference.h"

namespace Trick
{

    class ReferenceUtils
    {
        public:
            // Returns true when ref represents an access that went through an STL container:
            //   stl_present == 1  (STL indexed somewhere in the path, e.g. xxx[2].yyy.www)
            //   OR attr->type == TRICK_STL with the reference ending in ']' (e.g. vec[0])
            static bool is_stl_ref(const REF2* ref);

            // Returns the effective TRICK_TYPE for value interpretation:
            //   TRICK_STL whose reference ends with ']' -> attr->stl_elem_type
            //   everything else                         -> attr->type
            static TRICK_TYPE effective_trick_type(const REF2* ref);

            // Returns the effective byte size for buffer allocation / copying:
            //   TRICK_STL whose reference ends with ']' -> sizeof(stl_elem_type)
            //   everything else                         -> attr->size
            static size_t effective_trick_size(const REF2* ref);
    };

} // namespace Trick

#endif // REFERENCE_UTILS_HH
