#ifndef ATTRIBUTES_UTILS_HH
#define ATTRIBUTES_UTILS_HH

/*
    PURPOSE: ( Provides helper functions for working with ATTRIBUTES. )
*/

#include <cstdio>
#include <cstring>

#include "trick/attributes.h"

namespace Trick
{

    class AttributesUtils
    {
    public:
        // Compute the byte size occupied by a member - references, arrays, and pointers.
        static size_t compute_member_byte_size(const ATTRIBUTES &member)
        {
            // if mod bit 0 is set, member is a reference. Width of reference is stored
            // in bits 3-8 of mods. We could use size(void*), but that is implementation
            // specific and nto required by C++ standard.
            if ((member.mods & 1) == 1)
            {
                return ((member.mods >> 3) & 0x3F);
            }

            // if array
            if (member.num_index != 0)
            {
                // if size of last valid index is 0, then we are looking at a pointer
                if (member.index[member.num_index - 1].size == 0)
                {
                    return sizeof(void *);
                }
                size_t total = member.size;
                // multiply by sizes of fixed dimensions
                for (int i = 0; i < member.num_index; ++i)
                {
                    if (member.index[i].size != 0)
                    {
                        total *= member.index[i].size;
                    }
                    else
                    {
                        break;
                    }
                }
                return total;
            }

            // just return member.size otherwise
            return member.size;
        }

        // Return the ATTRIBUTES entry that contains the given offset within the struct/class
        static ATTRIBUTES *find_member_by_offset(ATTRIBUTES *structAttr, size_t addrOffsetFromStruct)
        {
            int ii = 0;
            size_t temp_size;
            /* Find the memeber which contains the address pointed to by addr and offset */
            while (structAttr[ii].name[0] != '\0')
            {
                temp_size = compute_member_byte_size(structAttr[ii]);
                if (addrOffsetFromStruct < structAttr[ii].offset ||
                    addrOffsetFromStruct >= structAttr[ii].offset + temp_size)
                {
                    ++ii;
                }
                else
                {
                    break;
                }
            }
            return &(structAttr[ii]);
        }

        // Count the number of fixed dimensions on an ATTRIBUTES entry.
        static int count_fixed_dims(const ATTRIBUTES &member)
        {
            int num_fixed_dims = 0;
            for (int i = 0; i < member.num_index; ++i)
            {
                if (member.index[i].size > 0)
                {
                    ++num_fixed_dims;
                }
                else
                {
                    break;
                }
            }
            return num_fixed_dims;
        }


        /**
         * Result of traversing a structure to find an attribute at a given address offset.
         * This structure contains the common information for traversal.
         */
        struct TraversalResult
        {
            ATTRIBUTES* found_attr;             // The attribute found at the given offset (or parent if not found)
            size_t offset_from_struct;          // Offset from the structure base address
            size_t offset_from_found_attr;      // Offset from the found attribute's address
            int array_indices[TRICK_MAX_INDEX]; // Computed array indices (if applicable)
            int num_computed_indices;           // Number of valid indices in array_indices
            bool is_exact_match;                // True if found_attr exactly matches the address
            bool is_in_anonymous_member;        // True if the address is in a **'d out member
        };

        /**
         * Traverse a structured type to find the attribute corresponding to a given address offset.
         * This function provides the common traversal logic that can be used for the need to 
         * traverse structures but expecting different output formats.
         * 
         * @param reference_offset Offset from struct_base_addr to the address we're looking for
         * @param struct_base_addr Base address of the structure
         * @param struct_attr ATTRIBUTES of the structure to traverse
         * @param result Output parameter containing traversal results
         * @return 0 if success, otherwise 1 or non-zero
         */
        static int traverse_for_offset(
            size_t reference_offset,
            size_t struct_base_addr,
            ATTRIBUTES *struct_attr,
            TraversalResult &result)
        {
            // Initialize result
            result.found_attr = nullptr;
            result.offset_from_struct = reference_offset;
            result.offset_from_found_attr = 0;
            result.num_computed_indices = 0;
            result.is_exact_match = false;
            result.is_in_anonymous_member = false;
            memset(result.array_indices, 0, sizeof(result.array_indices));

            // Find the structure member that corresponds to the reference offset
            ATTRIBUTES *ret_attr = find_member_by_offset(struct_attr, reference_offset);

            // Check if we failed to find a member (anonymous/**'d out member)
            if (ret_attr->name[0] == '\0')
            {
                result.is_in_anonymous_member = true;
                result.offset_from_found_attr = reference_offset;
                return 0;
            }

            result.found_attr = ret_attr;
            result.offset_from_found_attr = reference_offset - ret_attr->offset;

            // Handle non-structured (primitive) types
            if (ret_attr->type != TRICK_STRUCTURED)
            {
                // Check if it's a scalar or unconstrained pointer
                if (ret_attr->num_index == 0 || ret_attr->index[0].size == 0)
                {
                    result.is_exact_match = (result.offset_from_found_attr == 0);
                    return 0;
                }

                // It's an array - compute indices
                size_t offset_in_array = result.offset_from_found_attr;
                bool ok = compute_fixed_indices_for_linear_offset(*ret_attr, offset_in_array, result.array_indices, result.num_computed_indices);

                if (!ok)
                {
                    return 1;
                }

                return 0;
            }

            // Handle TRICK_STRUCTURED types

            // If it's a reference (&), we're done
            if ((ret_attr->mods & 1) == 1)
            {
                result.is_exact_match = true;
                return 0;
            }

            // If it's an unarrayed struct, caller should recurse into it
            if (ret_attr->num_index == 0)
            {
                // Signal to caller that recursion is needed
                // The found_attr and offset info are already set
                return 0;
            }

            // If it's a pointer (unconstrained array), we're done
            if (ret_attr->index[0].size == 0)
            {
                result.is_exact_match = true;
                return 0;
            }

            // It's an arrayed struct - compute indices
            size_t offset_in_array = result.offset_from_found_attr;
            bool ok = compute_fixed_indices_for_linear_offset(*ret_attr, offset_in_array, result.array_indices, result.num_computed_indices);

            if (!ok)
            {
                return 1;
            }

            // Calculate the linear offset within the array element
            size_t element_offset = 0;
            for (int j = 0; j < ret_attr->num_index; j++)
            {
                int m = result.array_indices[j];
                for (int k = j + 1; m && (k < ret_attr->num_index); k++)
                {
                    m *= ret_attr->index[k].size;
                }
                element_offset += m * ret_attr->size;
            }

            result.offset_from_found_attr = element_offset;

            return 0;
        }

    private:
        // Given a byte offset within a member's storage, compute indices for fixed dimensions.
        // Returns false if an invalid configuration is detected (e.g., divide-by-zero risk).
        static bool compute_fixed_indices_for_linear_offset(
            const ATTRIBUTES &member,
            long offset_within_member_bytes,
            int out_indices[TRICK_MAX_INDEX],
            int &out_num_fixed_dims)
        {
            out_num_fixed_dims = count_fixed_dims(member);
            if (out_num_fixed_dims == 0)
            {
                return true;
            }

            long size = member.size;
            int last_size = member.size;
            long remaining = offset_within_member_bytes;

            for (int i = out_num_fixed_dims - 1; i >= 0; --i)
            {
                size *= member.index[i].size;
                if (size == 0 || last_size == 0)
                {
                    return false;
                }
                out_indices[i] = static_cast<int>((remaining % size) / last_size);
                remaining -= last_size * out_indices[i];
                last_size = size;
            }
            return true;
        }
    };

} // namespace Trick

#endif // ATTRIBUTES_UTILS_HH