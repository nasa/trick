#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <sstream>

#include "trick/MemoryManager.hh"
#include "trick/vval.h"
#include "trick/attributes.h"
#include "trick/reference.h"
#include "trick/parameter_types.h"

/*
 Updates R, a reference to an arrayed object, to a reference to the indexed sub-element of that arrayed object.
*/
int Trick::MemoryManager::ref_dim( REF2* R, V_DATA* V) {

    int jj;
    int item_size;
    int index_value = vval_int(V);

    if (R->ref_type != REF_ADDRESS) {
        emitError("Attempt to index into a non-address reference is bogus in ref_dim.") ;
        return (1);
    }

    R->num_index_left--;
    if (R->num_index_left < 0) {
        /* if we have too many dimensions, flag an error */
        emitError("Too many dimensions in ref_dim.\n") ;
        return (TRICK_PARAMETER_ARRAY_SIZE);
    }

    /*Calculate the size of the items in this array. */
    item_size =  R->attr->size;
    for (jj = (R->attr->num_index - 1); jj > R->num_index; jj--) {
        if (R->attr->index[jj].size > 0) {
            item_size *= R->attr->index[jj].size;
        } else {
            item_size = sizeof(void *);
            R->pointer_present = 1 ;
        }
    }

    /* if current dimension is a constrained ... */
    if (R->attr->index[R->num_index].size != 0) {

        /* for constrained dimensions, we can check the validity of the index value */
        if (index_value >= R->attr->index[R->num_index].size || index_value < 0) {
            /* print out of bounds error message if MM debug_level is greater than 1 */
            if (debug_level > 1) {
                emitError("Memory Manager ERROR: Array index out of bounds.") ;
            }
            return (TRICK_PARAMETER_ARRAY_SIZE);
        }

    } else {

        /* for unconstrained dimensions, we can check that the index value is non-negative
           and that it is less than the size of the array */
        if (index_value >= (get_size(*(void**)(R->address))) || index_value < 0) {

            if (index_value >= 0 && get_size(*(void**)(R->address)) == 0) {
                // Special case (do nothing here):
                //   For regular pointers as an equivalent to ->
                //   For array pointers that are not yet allocated
                // If a pointer is not allocated regardless of regular pointer or array pointer, an error message will be emitted below
                //   However, if the pointer is already assigned to a valid address, the error message will NOT be emitted below
            } else {
                /* print out of bounds error message if MM debug_level is greater than 1 */
                if (debug_level > 1) {
                    std::stringstream message;
                    message << index_value << " is out of bounds for " << R->reference << " (size=" << get_size(*(void**)(R->address)) << ").";
                    emitError(message.str());
                }
                return (TRICK_PARAMETER_ARRAY_SIZE);
            }
        }

        R->pointer_present = 1 ;
        if ( R->create_add_path ) {
            ADDRESS_NODE * address_node ;

            address_node = new ADDRESS_NODE ;
            address_node->operator_ = AO_DEREFERENCE ;
            address_node->operand.address = NULL ;
            DLL_AddTail(address_node , R->address_path) ;
        }

        // Dereference the pointer.
        R->address = *(void**)R->address;
        if ( R->address == NULL) {
            std::stringstream message;
            message << "Reference (" << R->reference << ") address is NULL in ref_dim.";
            emitError(message.str());
            return(TRICK_PARAMETER_ADDRESS_NULL) ;
        }

        // Get allocation information for the address
        ALLOC_INFO *alloc_info = get_alloc_info_of(R->address);

        // Skip if allocation name is NULL
        if (alloc_info->name != NULL) {
            // Get the reference name from the address that R points if exists and contains & at front
            // Otherwise, the pointer is not assigned to anything else rather itself is allocated
            std::string ref_name = ref_name_from_address(R->address);
            if (!ref_name.empty() && ref_name.front() == '&') {
                ref_name = ref_name.substr(1);

                // Get the reference attributes for what the pointer points to
                REF2 *ref2 = ref_attributes((char*)ref_name.c_str());

                // Check if the reference that the pointer points to is valid. Return error if not.
                if (ref2 == NULL) {
                    std::stringstream message;
                    message << "Reference (" << R->reference << ") is not allocated in ref_dim.";
                    emitError(message.str());
                    return(TRICK_PARAMETER_ADDRESS_NULL);
                }

                // Check if the pointer points to a static array or a dynamic array and if array index is out of bounds
                if (ref2->attr && ref2->attr->index[ref2->attr->num_index-1].size != 0) { // Static array case
                    // Check if the index is out of bounds if the pointer points to a static array
                    if (index_value >= ref2->attr->index[ref2->attr->num_index-1].size || index_value < 0) {
                        /* print out of bounds error message if MM debug_level is greater than 1 */
                        if (debug_level > 1) {
                            emitError("Memory Manager ERROR: Array index out of bounds.") ;
                        }
                        free(ref2);
                        return (TRICK_PARAMETER_ARRAY_SIZE);
                    }
                } else { // Dynamic array case
                    // Check if the index is out of bounds if the pointer points to a dynamic array
                    if (index_value >= (get_size(*(void**)(R->address)))) {
                        /* print out of bounds error message if MM debug_level is greater than 1 */
                        if (debug_level > 1) {
                            std::stringstream message;
                            message << index_value << " is out of bounds for " << R->reference << " (size=" << get_size(*(void**)(R->address)) << ").";
                            emitError(message.str());
                        }
                        free(ref2);
                        return (TRICK_PARAMETER_ARRAY_SIZE);
                    }
                }
            } // if (!ref_name.empty() && ref_name.front() == '&') {
        } // if (alloc_info->name != NULL) {
    } // if (R->attr->index[R->num_index].size != 0) {

    if ( R->create_add_path ) {

        ADDRESS_NODE * address_node ;

        if ( index_value > 0 ) {
            address_node = (ADDRESS_NODE *)DLL_GetAt(DLL_GetTailPosition(R->address_path), R->address_path) ;
            switch ( address_node->operator_ ) {
                case AO_ADDRESS:
                    address_node->operand.address = (void *)((char *)address_node->operand.address +  index_value * item_size) ;
                    break ;
                case AO_DEREFERENCE:
                    address_node = new ADDRESS_NODE ;
                    address_node->operator_ = AO_OFFSET ;
                    address_node->operand.offset = index_value * item_size ;
                    DLL_AddTail(address_node , R->address_path) ;
                    break ;
                case AO_OFFSET:
                    address_node->operand.offset += index_value * item_size ;
                    break ;
            }
        }
    }

    R->address =  (void*)((char*)R->address + index_value * item_size);
    R->num_index++;

    return (TRICK_NO_ERROR);
}
