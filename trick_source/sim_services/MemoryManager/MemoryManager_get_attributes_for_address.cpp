#include "trick/MemoryManager.hh"
#include "trick/ClassicCheckPointAgent.hh"
#include "trick/AttributesUtils.hh"

static int getCompositeSubReference(
    size_t addrValue,
    ATTRIBUTES &attrOut,
    size_t structAddrValue,
    ATTRIBUTES *structAttr)
{
    if (addrValue < structAddrValue)
    {
        return 1;
    }

    size_t addrOffsetFromStruct = addrValue - structAddrValue;

    Trick::AttributesUtils::TraversalResult traversalResult;

    int ret = Trick::AttributesUtils::traverse_for_offset(addrOffsetFromStruct, structAddrValue, structAttr, traversalResult);

    if (ret != 0)
    {
        return 1;
    }

    // Handle anonymous/**'d out members
    if (traversalResult.is_in_anonymous_member)
    {
        attrOut = ATTRIBUTES();
        attrOut.type = TRICK_VOID_PTR;
        attrOut.size = sizeof(void *);
        attrOut.offset = addrOffsetFromStruct;
        attrOut.num_index = 1;
        attrOut.index[0].size = 0;

        return 0;
    }

    ATTRIBUTES *retAttr = traversalResult.found_attr;

    // If the found attribute is a primitive type
    if (retAttr->type != TRICK_STRUCTURED)
    {
        // Scalar or pointer - return as unconstrained pointer
        if (retAttr->index[0].size == 0)
        {
            attrOut = *retAttr;
            attrOut.num_index = 1;
            attrOut.index[0].size = (retAttr->num_index == 0) ? 1 : 0;
            return 0;
        }

        // This is an array type. Calculate the offset from the address to the end of the attribute
        // Then, calculate the remaining elements in the array as a single value.
        // Calculate the maximum offset of this attribute.
        size_t max_offset = retAttr->size;
        for (int ii = 0; ii < retAttr->num_index; ii++)
        {
            if (retAttr->index[ii].size > 0)
            {
                max_offset *= retAttr->index[ii].size;
            }
        }

        attrOut = *retAttr;
        attrOut.num_index = 1;
        attrOut.index[0].size = (max_offset - traversalResult.offset_from_found_attr) / retAttr->size;
        return 0;
    }

    // If it is a reference type, do nothing and return
    if ((retAttr->mods & 1) == 1)
    {
        attrOut = *retAttr;
        return 0;
    }

    // If attribute is an unarrayed struct, continue to call getCompositeSubReference
    if (retAttr->num_index == 0)
    {
        return getCompositeSubReference(addrValue, attrOut, structAddrValue + retAttr->offset, (ATTRIBUTES *)retAttr->attr);
    }

    // If the member is a pointer, do nothing and return
    if (retAttr->index[0].size == 0)
    {
        attrOut = *retAttr;
        return 0;
    }

    // Arrayed struct - recurse into the element
    return getCompositeSubReference(addrValue, attrOut,
                                    structAddrValue + retAttr->offset + traversalResult.offset_from_found_attr,
                                    (ATTRIBUTES *)retAttr->attr);
}

/**
 @par Detailed Description:
 */
void Trick::MemoryManager::get_attributes_for_address(void *address, ATTRIBUTES &attrOut)
{
    /** Find the allocation that contains the pointer-address. */
    ALLOC_INFO *alloc_info = get_alloc_info_of(address);

    if (alloc_info != NULL)
    {
        // Found the allocation. Look for the attribute that pertains to it.
        size_t addrValue = reinterpret_cast<size_t>(address);
        size_t allocAddrStart = reinterpret_cast<size_t>(alloc_info->start);
        size_t alloc_elem_size = alloc_info->size;
        size_t alloc_elem_index = (addrValue - allocAddrStart) / alloc_elem_size;
        size_t misalignment = (addrValue - allocAddrStart) % alloc_elem_size;

        if (alloc_info->type == TRICK_STRUCTURED)
        {
            // The allocation is not a primitive type, traverse the structure unil the attribute is found or an
            // anonymous pointer is returned
            getCompositeSubReference(addrValue, attrOut, allocAddrStart + (alloc_elem_index * alloc_elem_size),
                                     alloc_info->attr);
        }
        else
        {
            // Primitive allocation is found, compute the remaining number of elements from the current address
            size_t allocAddrEnd = reinterpret_cast<size_t>(alloc_info->end) + 1;
            size_t max_alloc_index = (allocAddrEnd - allocAddrStart) / alloc_elem_size;
            if (alloc_info->attr)
            {
                // If there's an attribute for this allocation for some reason, populate the return structure.
                attrOut = *(alloc_info->attr);
            }
            attrOut.num_index = 1;
            attrOut.index[0].size = max_alloc_index - alloc_elem_index;
        }
    }
    else
    {
        // No allocation found, num_index = 0 should indicate that it's not found. On the swig, side, the calling
        // function will set num_index to 1 anyway and it becomes a generic pointer according to swig/python.
        attrOut = ATTRIBUTES();
    }
}
