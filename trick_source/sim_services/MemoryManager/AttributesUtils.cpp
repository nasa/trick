#include "trick/AttributesUtils.hh"

// Compute the byte size occupied by a member - references, arrays, and pointers
size_t Trick::AttributesUtils::compute_member_byte_size(const ATTRIBUTES &member)
{
    // if mod bit 0 is set, member is a reference. Width of reference is stored
    // in bits 3-8 of mods. We could use size(void*), but that is implementation
    // specific and not required by C++ standard.
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

// Return the ATTRIBUTES entry that contains the given offset within the struct/class.
// Example: for a struct with members like this:
//   struct MyStruct {
//     int a;          // offset 0, size 4 (padding 4-7)
//     double b;       // offset 8, size 8
//     char c[10];     // offset 16, size 10
//   }
// Then:
//   find_member_by_offset(structAttr, 2) would return the ATTRIBUTES for 'a' because offset 2 is within 'a' (0-3).
//   find_member_by_offset(structAttr, 10) would return the ATTRIBUTES for 'b' because offset 10 is within 'b' (8-15).
//   find_member_by_offset(structAttr, 20) would return the ATTRIBUTES for 'c' because offset 20 is within 'c' (16-25).
ATTRIBUTES *Trick::AttributesUtils::find_member_by_offset(ATTRIBUTES *structAttr, size_t addrOffsetFromStruct)
{
    int ii = 0;
    size_t temp_size;
    // Find the member which contains the address pointed to by addr and offset
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
int Trick::AttributesUtils::count_fixed_dims(const ATTRIBUTES &member)
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

// Resolve a byte offset within a structured object to the matching member and array context.
// This call resolves one strucural level at a time. The caller is responsible for stepping
// through nested structures by calling this function recursively with the found member's ATTRIBUTES
// and the offset within that member.
//
// Example 1: for a struct with members like this:
//   struct MyStruct {
//     int a;        // offset 0, size 4
//     int b[3];     // offset 4, size 4 each
//   }
// Entries would be:
//   myStructAttr[0] = { name="a", offset=0, size=4, num_index=0, ... }
//   myStructAttr[1] = { name="b", offset=4, size=4, num_index=1, index[0].size=3, ... }
//   myStructAttr[2] = { name="", ... } // Sentinel
// So member b occupies these bytes:
//   b[0] at offset 4-7
//   b[1] at offset 8-11
//   b[2] at offset 12-15
// Then call:
//   TraversalResult result;
//   traverse_for_offset(10, myStructAttr, result);
// Basically asking given the member layout of MyStruct, what member contains byte offset 10
//   The function would resolve offset 10 to member 'b' and compute the array index as 1
//   because b[1] occupies bytes 8-11 which contains byte 10. So result would be:
//     result.found_attr = b (pointer to myStructAttr[1] (the ATTRIBUTES for 'b'))
//     result.offset_from_found_attr = 10 - 4 = 6 (offset within member 'b')
//     result.array_indices = [1] (index for the first dimension of 'b')
//     result.num_computed_indices = 1 (because 'b' is a 1D array)
//     result.is_in_anonymous_member = false (because a member with a name that contains the offset was found)
//
// Example 2: for a struct with a structured array member like this:
//   struct Inner {
//     double q;   // offset 0, size 8
//     int r;      // offset 8, size 4
//   };
//
//   struct Outer {
//     int a;          // offset 0
//     Inner items[4]; // offset 8, each element size 16
//   };
//
// Array layout of items would be:
//   items[0].q at offset 8-15
//   items[0].r at offset 16-19
//   items[1].q at offset 24-31
//   items[1].r at offset 32-35
//   items[2].q at offset 40-47
//   items[2].r at offset 48-51
//   items[3].q at offset 56-63
//   items[3].r at offset 64-67
//
// If called with reference_offset = 44, the function would resolve it to the
// named member 'items', because offset 44 lies within the items member.
// The result would be:
//   result.found_attr = items (pointer to ATTRIBUTES for 'items')
//   result.offset_from_found_attr = 32
//     (initially 44 - 8 = 36, then adjusted to the base offset of items[2],
//      which is 2 * 16 = 32 bytes into the items array)
//   result.array_indices = [2]
//   result.num_computed_indices = 1
//   result.is_in_anonymous_member = false
//
// The call resolves the address to the named member 'items' and computes array
// index 2 because items[2] contains byte offset 44. It does not resolve to the
// inner members 'q' or 'r' yet; that happens after the caller recurses into
// items[2]. On that recursive call, the inner offset is 44 - 40 = 4, so the
// found_attr would be 'q' because q occupies offsets 0-7 within Inner.
// That is how ClassicCheckPointerAgent builds names, and how MemoryManager_get_attributes_for_address
// walks into nested members.
int Trick::AttributesUtils::traverse_for_offset(
    size_t reference_offset,
    ATTRIBUTES *struct_attr,
    TraversalResult &result)
{
    // Initialize result
    result = TraversalResult{};

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

// Convert a byte offset within fixed array into concrete array indices.
// Example: For an array int a[3][4], assuming:
//   member.size = 4 (size of int in bytes)
//   dimensions are 3 and 4 (member.index[0].size = 3, member.index[1].size = 4)
//   offset_within_member_bytes = 28
//     Byte 28 means the 8th int overall, so the expected indices would be [1][3] 
//     because a[1][3] is the 8th int (0-based indexing).
//   So, out_indices would be set to [1, 3] and out_num_fixed_dims would be set to 2.
bool Trick::AttributesUtils::compute_fixed_indices_for_linear_offset(
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