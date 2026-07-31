#ifndef ATTRIBUTES_UTILS_HH
#define ATTRIBUTES_UTILS_HH

/*
    PURPOSE: ( Provides helper functions for working with ATTRIBUTES. )
*/

#include "trick/attributes.h"

namespace Trick
{

class AttributesUtils
{
public:
    // Compute the byte size occupied by a member - references, arrays, and pointers.
    static size_t compute_member_byte_size(const ATTRIBUTES& member);

    // Return the ATTRIBUTES entry that contains the given offset within the struct/class.
    static ATTRIBUTES* find_member_by_offset(ATTRIBUTES* structAttr, size_t addrOffsetFromStruct);

    // Count the number of fixed dimensions on an ATTRIBUTES entry.
    static int count_fixed_dims(const ATTRIBUTES& member);

    /**
     * Result of traversing a structure to find an attribute at a given address offset.
     * This structure contains the common information for traversal.
     */
    struct TraversalResult
    {
        ATTRIBUTES* found_attr = nullptr; // The attribute found at the given offset, or nullptr
                                          // if the offset falls in an anonymous/hidden member region
        size_t offset_from_found_attr = 0; // Offset relative to the found attribute; for structured arrays,
                                           // this is adjusted to the selected element base.
        int array_indices[TRICK_MAX_INDEX] = { }; // Computed array indices (if applicable)
        int num_computed_indices = 0; // Number of valid indices in array_indices
        bool is_in_anonymous_member = false; // True if the offset falls in an anonymous or otherwise unnamed member region
    };

    /**
     * Traverse a structured type to find the attribute corresponding to a given address offset.
     * This function provides the common traversal logic that can be used for the need to
     * traverse structures but expecting different output formats.
     *
     * @param reference_offset Offset from the structure base address to the address we're looking for
     * @param struct_attr ATTRIBUTES of the structure to traverse
     * @param result Output parameter containing traversal results
     * @return 0 if success, otherwise 1 or non-zero
     */
    static int traverse_for_offset(
        size_t reference_offset,
        ATTRIBUTES* struct_attr,
        TraversalResult& result);

    // Given a byte offset within a member's storage, compute indices for fixed dimensions.
    // Returns false if an invalid configuration is detected (e.g., divide-by-zero risk).
    static bool compute_fixed_indices_for_linear_offset(
        const ATTRIBUTES& member,
        long offset_within_member_bytes,
        int out_indices[TRICK_MAX_INDEX],
        int& out_num_fixed_dims);
};

} // namespace Trick

#endif // ATTRIBUTES_UTILS_HH