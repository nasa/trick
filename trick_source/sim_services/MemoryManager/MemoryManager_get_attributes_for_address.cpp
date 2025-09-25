#include "trick/MemoryManager.hh"
#include "trick/ClassicCheckPointAgent.hh"

static ATTRIBUTES *findMember(ATTRIBUTES *A, long referenceOffset)
{
    int i = 0;
    int j;
    int temp_size;
    /* Find the member which contains the address pointed to by rAddr */
    while ((A[i].name[0] != '\0'))
    {

        // if mod bit 0 is set, A[i] is a reference. Width of reference is stored
        // in mod bits 3-8. We could use sizeof(void*), but that is implementation
        // specific and not required by C++ standard.
        if ((A[i].mods & 1) == 1)
        {
            temp_size = ((A[i].mods >> 3) & 0x3F);
            // Calculate the size (temp_size) of the referenced member variable.
        }
        else if (A[i].num_index != 0)
        {
            // if size of last valid index is 0, then we are looking at a pointer
            if (A[i].index[A[i].num_index - 1].size == 0)
            {
                temp_size = sizeof(void *);
            }
            else
            {
                temp_size = A[i].size;
            }
            for (j = 0; j < A[i].num_index; j++)
            {
                if (A[i].index[j].size != 0)
                {
                    temp_size *= A[i].index[j].size;
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            temp_size = A[i].size;
        }

        // if the reference is not to this member variable, move on to the next member variable.
        if ((referenceOffset < (long)(A[i].offset)) ||
            (referenceOffset >= (long)(A[i].offset + temp_size)))
        {
            i++;
        }
        else
        {
            break;
        }
    }

    return &(A[i]);
}

static int getCompositeSubReference(
    void *reference_address, /* Address we are looking for */
    ATTRIBUTES &left_type,   /* Attributes of type we are looking for */
    void *structure_address, /* Address of struct we are in */
    ATTRIBUTES *A)
{
    // int j, m;
    // long offset;
    int my_index[9] = {};
    // int ret;
    int size, last_size;

    char *rAddr = (char *)reference_address;
    char *sAddr = (char *)structure_address;

    long referenceOffset = (long)rAddr - (long)sAddr;

    // selected ATTRIBUTES stucture from A (singular)
    ATTRIBUTES *Ai;

    if (referenceOffset < 0)
    {
        // message_publish(MSG_ERROR, "Checkpoint Agent ERROR: Address to find is less than struct address.\n");
        return 1;
    }

    // Find the structure member that corresponds to the reference address.
    // If name is empty, we have failed.
    Ai = findMember(A, referenceOffset);

    /******If failed to find member, set reference_name to offset only and return ****/
    if (Ai->name[0] == '\0')
    {
        /* If we fail to find a member corresponding to the reference address,
           it must mean that the ATTRIBUTES don't contain a description for
           that particular member, i.e., it was **'ed out. In this case, we can only
           represent the reference_address as a byte offset into the structure.
         */
        if (referenceOffset >= 0)
        {
            left_type = ATTRIBUTES();
            left_type.num_index = 1;
            left_type.index[0].size = 0;
        }
        else
        {
            return 1; // ERROR
        }
        return 0;
    }

    /******************************************************************************/
    /* If the referenced member variable is an intrinsic type */
    if (Ai->type != TRICK_STRUCTURED)
    {

        /* If the reference address is non-array or a pointer, return reference_name as is */
        if ((Ai->num_index == 0) || (Ai->index[0].size == 0))
        {
            left_type = ATTRIBUTES();
            left_type.num_index = 1;
            left_type.index[0].size = 0;
            return 0;
        }

        /* else, rAddr is pointing to an array, determine its dimensions and determine
           the element pointed to by rAddr. Then print the index and return */

        long offset = (long)rAddr - ((long)sAddr + Ai->offset);
        long max_offset = Ai->size;
        // size = last_size = Ai->size;

        /* Calculate the number of fixed dimensions. */
        // int num_fixed_dims = 0;
        for (int j = 0; j < Ai->num_index; j++)
        {
            if (Ai->index[j].size > 0)
            {
                max_offset *= Ai->index[j].size;
            }
        }

        left_type = ATTRIBUTES();
        left_type.num_index = 1;
        left_type.index[0].size = (max_offset - offset) / Ai->size;

        return 0;
    }
    /******** TRICK_STRUCTURED ****************************************************/
    /* if it is a reference, do nothing and return */
    if ((Ai->mods & 1) == 1)
    { // Ai->type == TRICK_STRUCTURED
        return 0;
    }
    /*if member is an unarrayed struct, continue to call getCompositeSubReference.*/
    if (Ai->num_index == 0)
    {
        int ret = getCompositeSubReference(rAddr, left_type, sAddr + Ai->offset, (ATTRIBUTES *)Ai->attr);

        if (ret != 0)
        {
            return 1; // ERROR.
        }
        return 0;
    }

    /***** If the member is not a pointer do nothing and return *******************/
    if (Ai->index[0].size == 0)
    {
        return 0;
    }

    /*********** Member is an arrayed struct **************************************/

    long offset = (long)rAddr - ((long)sAddr + Ai->offset);
    size = last_size = Ai->size;

    /* Calculate the indices into the array */
    for (int j = Ai->num_index - 1; j >= 0; j--)
    {
        if (Ai->index[j].size != 0)
        {
            size *= Ai->index[j].size;
        }
        my_index[j] = (int)((offset % size) / last_size);
        offset -= last_size * my_index[j];
        last_size = size;
    }

    // for (int j = 0; j < Ai->num_index; j++)
    // {
    //     size_t len = strlen(reference_name);
    //     size_t rem = (size_t)256 - len;
    //     snprintf(&reference_name[len], rem, "[%d]", my_index[j]);
    // }

    /* if left_type specifies the current member, stop here */
    // if ((left_type != NULL) && (*left_type != NULL) && (Ai->attr == (*left_type)->attr))
    // {
    //     return 0;
    // }

    /**** Go find the subreference for the arrayed struct member and append *********/

    /* get the offset into the array that rAddr points to */
    offset = 0;
    for (int j = 0; j < Ai->num_index; j++)
    {
        int m = my_index[j];
        for (int k = j + 1; m && (k < Ai->num_index); k++)
        {
            m *= Ai->index[k].size;
        }
        offset += m * Ai->size;
    }

    {
        int ret = getCompositeSubReference(rAddr, left_type, sAddr + Ai->offset + offset, (ATTRIBUTES *)Ai->attr);

        if (ret != 0)
        {
            return 1; // ERROR
        }
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
        size_t alloc_elem_size;
        size_t alloc_elem_index;
        size_t misalignment;

        alloc_elem_size = alloc_info->size;
        alloc_elem_index = ((size_t)address - (size_t)alloc_info->start) / alloc_elem_size;
        misalignment = ((size_t)address - (size_t)alloc_info->start) % alloc_elem_size;

        if (alloc_info->type == TRICK_STRUCTURED)
        {
            getCompositeSubReference(address,
                                     attrOut,
                                     (char *)alloc_info->start + (alloc_elem_index * alloc_info->size),
                                     alloc_info->attr);
        }
        else
        {
            size_t max_alloc_index = ((size_t)alloc_info->end - (size_t)alloc_info->start) / alloc_elem_size;
            attrOut = *(alloc_info->attr);
            attrOut.num_index = 1;
            attrOut.index[0].size = max_alloc_index - alloc_elem_index;
        }
    }
    else
    {
        attrOut = ATTRIBUTES();
        attrOut.num_index = 1;
        attrOut.index[0].size = 0;
    }
}
