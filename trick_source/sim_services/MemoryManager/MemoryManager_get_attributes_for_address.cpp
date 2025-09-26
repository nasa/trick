#include "trick/MemoryManager.hh"
#include "trick/ClassicCheckPointAgent.hh"

static ATTRIBUTES *findMember(ATTRIBUTES *structAttr, size_t addrOffsetFromStruct)
{
    int ii = 0;
    size_t temp_size;
    // Find the attribute that contains the address
    while ((structAttr[ii].name[0] != '\0'))
    {

        // if mod bit 0 is set, A[i] is a reference. Width of reference is stored
        // in mod bits 3-8. We could use sizeof(void*), but that is implementation
        // specific and not required by C++ standard.
        if ((structAttr[ii].mods & 1) == 1)
        {
            temp_size = ((structAttr[ii].mods >> 3) & 0x3F);
            // Calculate the size (temp_size) of the referenced member variable.
        }
        else if (structAttr[ii].num_index != 0)
        {
            // if size of last valid index is 0, then we are looking at a pointer
            if (structAttr[ii].index[structAttr[ii].num_index - 1].size == 0)
            {
                temp_size = sizeof(void *);
            }
            else
            {
                temp_size = structAttr[ii].size;
            }
            for (int jj = 0; jj < structAttr[ii].num_index; ++jj)
            {
                if (structAttr[ii].index[jj].size != 0)
                {
                    temp_size *= structAttr[ii].index[jj].size;
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            temp_size = structAttr[ii].size;
        }

        // if the reference is not to this member variable, move on to the next member variable.
        if ((addrOffsetFromStruct < (size_t)(structAttr[ii].offset)) ||
            (addrOffsetFromStruct >= (size_t)(structAttr[ii].offset + temp_size)))
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

static int getCompositeSubReference(
    size_t addrValue,
    ATTRIBUTES &attrOut,  
    size_t structAddrValue,  
    ATTRIBUTES *structAttr)
{
    // Make sure the address of interset is greater than starting address of this structure
    if(addrValue < structAddrValue) {
        return 1;
    }

    size_t addrOffsetFromStruct = addrValue - structAddrValue;

    // Find the structure member that corresponds to the reference address.
    ATTRIBUTES * retAttr = findMember(structAttr, addrOffsetFromStruct);

    // If the name field of the returned attribute is empty, this means that the ATTRIBUTES don't contain a description 
    // for the specific address, i.e. it was **'d out. Returne a void pointer attribute with the offset from this 
    // structure
    if (retAttr->name[0] == '\0')
    {
        attrOut = ATTRIBUTES();
        attrOut.type = TRICK_VOID_PTR;
        attrOut.size = sizeof(void *);
        attrOut.offset = addrOffsetFromStruct;
        attrOut.num_index = 1;
        attrOut.index[0].size = 0;

        return 0;
    }

    // If the found attribute is a primitive type
    if (retAttr->type != TRICK_STRUCTURED)
    {
        // If the reference address is non-array or a pointer, return the primitive type but unconstrained pointer
        if (retAttr->index[0].size == 0)
        {
            if (retAttr->num_index == 0)
            {
                // Scalar
                attrOut = *retAttr;
                attrOut.num_index = 1;
                attrOut.index[0].size = 1;
            } else {
                // Pointer
                attrOut = *retAttr;
                attrOut.num_index = 1;
                attrOut.index[0].size = 0;
            }
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

        size_t addrOffsetFromAttr = addrValue - (structAddrValue + retAttr->offset);

        attrOut = *retAttr;
        attrOut.num_index = 1;
        attrOut.index[0].size = (max_offset - addrOffsetFromAttr) / retAttr->size;

        return 0;
    }
    
    // If it is a reference, do nothing and return */
    if ((retAttr->mods & 1) == 1)
    { 
        attrOut = *retAttr;
        
        return 0;
    }

    // If attribute is an unarrayed struct, continue to call getCompositeSubReference
    if (retAttr->num_index == 0)
    {
        int ret = getCompositeSubReference(addrValue, attrOut, structAddrValue + retAttr->offset, (ATTRIBUTES *)retAttr->attr);

        if (ret != 0)
        {
            return 1; // ERROR.
        }
        return 0;
    }

    // If the member is a pointer, do nothing and return
    if (retAttr->index[0].size == 0)
    {
        return 0;
    }

    // Attribute is an arrayed struct, Calculate the offset within the array from the attribute
    size_t max_offset = retAttr->size;
    for (int ii = 0; ii < retAttr->num_index; ii++)
    {
        if (retAttr->index[ii].size > 0)
        {
            max_offset *= retAttr->index[ii].size;
        }
    }

    size_t addrOffsetFromAttr = addrValue - (structAddrValue + retAttr->offset);

    int ret = getCompositeSubReference(addrValue, attrOut, addrOffsetFromAttr, (ATTRIBUTES *)retAttr->attr);

    if (ret != 0)
    {
        return 1; // ERROR
    }

    return 0;
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
            getCompositeSubReference(addrValue,
                                     attrOut,
                                     allocAddrStart + (alloc_elem_index * alloc_elem_size),
                                     alloc_info->attr);
        }
        else
        {
            // Primitive allocation is found, compute the remaining number of elements from the current address
            size_t allocAddrEnd = reinterpret_cast<size_t>(alloc_info->end)+1;
            size_t max_alloc_index = (allocAddrEnd - allocAddrStart) / alloc_elem_size;
            if(alloc_info->attr) {
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
