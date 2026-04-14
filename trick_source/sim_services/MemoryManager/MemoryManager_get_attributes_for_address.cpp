#include "trick/AttributesUtils.hh"
#include "trick/ClassicCheckPointAgent.hh"
#include "trick/MemoryManager.hh"

#include <string.h>

static int getCompositeSubReference(
    size_t addrValue,
    ATTRIBUTES& attrOut,
    size_t structAddrValue,
    ATTRIBUTES* structAttr,
    size_t& remainingOffset)
{
    if (addrValue < structAddrValue)
    {
        return 1;
    }

    size_t addrOffsetFromStruct = addrValue - structAddrValue;

    Trick::AttributesUtils::TraversalResult traversalResult;

    int ret = Trick::AttributesUtils::traverse_for_offset(addrOffsetFromStruct, structAttr, traversalResult);
    remainingOffset = traversalResult.offset_from_found_attr;

    if (ret != 0)
    {
        return 1;
    }

    // Handle anonymous/**'d out members
    if (traversalResult.is_in_anonymous_member)
    {
        attrOut = ATTRIBUTES();
        attrOut.type = TRICK_VOID_PTR;
        attrOut.size = sizeof(void*);
        attrOut.offset = addrOffsetFromStruct;
        attrOut.num_index = 1;
        attrOut.index[0].size = 0;

        return 0;
    }

    ATTRIBUTES* retAttr = traversalResult.found_attr;

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

        attrOut = *retAttr;
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
        return getCompositeSubReference(addrValue, attrOut, structAddrValue + retAttr->offset, (ATTRIBUTES*)retAttr->attr, remainingOffset);
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
        (ATTRIBUTES*)retAttr->attr, remainingOffset);
}

/**
 * Given an address, populate the attributes instance describing the properties of the address and any
 * remaining offset to the address so that the attributes pertain to the starting address of that attribute
 * and the offset is some array element or subcomponent within that attribute.
 * @param address pointer to the address of interest
 * @param attrOut reference to the ATTRIBUTES instance to be populated
 * @param remainingOffset reference to the size_t to be populate with the number of bytes to the start of
 *                        the attributes returned. (i.e. attrStartAddr = address - remainingOffset )
 */
void Trick::MemoryManager::get_attributes_for_address(void* address, ATTRIBUTES& attrOut, size_t& remainingOffset)
{
    if (address == nullptr)
    {
        attrOut = ATTRIBUTES();
        return;
    }

    /** Find the allocation that contains the pointer-address. */
    ALLOC_INFO* alloc_info = get_alloc_info_of(address);

    if (alloc_info != NULL)
    {
        // Found the allocation. Look for the attribute that pertains to it.
        size_t addrValue = reinterpret_cast<size_t>(address);
        size_t allocAddrStart = reinterpret_cast<size_t>(alloc_info->start);
        remainingOffset = addrValue - allocAddrStart;
        size_t alloc_elem_size = alloc_info->size;
        size_t alloc_elem_index = (addrValue - allocAddrStart) / alloc_elem_size;
        // size_t misalignment = (addrValue - allocAddrStart) % alloc_elem_size;

        if (alloc_info->type == TRICK_STRUCTURED)
        {
            // The allocation is not a primitive type, traverse the structure unil the attribute is found or an
            // anonymous pointer is returned
            getCompositeSubReference(addrValue, attrOut, allocAddrStart + (alloc_elem_index * alloc_elem_size),
                alloc_info->attr, remainingOffset);
        }
        else
        {
            // Primitive allocation is found, compute the remaining number of elements from the current address
            size_t allocAddrEnd = reinterpret_cast<size_t>(alloc_info->end) + 1;
            size_t offsetFromStart = addrValue - allocAddrStart;
            // size_t max_alloc_index = (allocAddrEnd - allocAddrStart) / alloc_elem_size;
            if (alloc_info->attr && alloc_info->type != TRICK_ENUMERATED)
            {
                // If there's an attribute for this allocation for some reason, populate the return structure.
                attrOut = *(alloc_info->attr);
            }
            else
            {
                attrOut.type = alloc_info->type;
                if (alloc_info->name)
                {
                    attrOut.name = strdup(alloc_info->name);
                }
                attrOut.stl_type = TRICK_STL_UNKNOWN;
                attrOut.num_index = alloc_info->num_index;
                for (int ii = 0; ii < alloc_info->num_index; ++ii)
                {
                    attrOut.index[ii].size = alloc_info->index[ii];
                }
            }
        }
    }
    else
    {
        // No allocation found, num_index = 0 should indicate that it's not found. On the swig, side, the calling
        // function will set num_index to 1 anyway and it becomes a generic pointer according to swig/python.
        attrOut = ATTRIBUTES();
    }
}
