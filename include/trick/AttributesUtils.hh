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
        static int compute_member_byte_size(const ATTRIBUTES &member)
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
                int total = member.size;
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
        static ATTRIBUTES *find_member_by_offset(ATTRIBUTES *structAttr, long addrOffsetFromStruct)
        {
            int ii = 0;
            long temp_size;
            /* Find the memeber which contains the address pointed to by addr and offset */
            while (structAttr[ii].name[0] != '\0')
            {
                temp_size = (long)(compute_member_byte_size(structAttr[ii]));
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

        // Given a byte offset within a member's storage, compute indices for fixed dimensions.
        // Returns false if an invalid configuration is detected (e.g., divide-by-zero risk).
        static bool compute_fixed_indices_for_linear_offset(const ATTRIBUTES &member,
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