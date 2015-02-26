/* 
 * $Id: MemoryManager_ref_dim.cpp 3627 2014-08-12 23:21:09Z jpenn1 $
 */

#include <stdio.h>
#include <sys/types.h>
#include <string.h>

#include "sim_services/MemoryManager/include/MemoryManager.hh"
#include "sim_services/MemoryManager/include/vval.h"
#include "sim_services/MemoryManager/include/attributes.h"
#include "sim_services/MemoryManager/include/reference.h"
#include "sim_services/MemoryManager/include/parameter_types.h"
#include "sim_services/MemoryManager/include/message_proto.h"

/*
 Updates R, a reference to an arrayed object, to a reference to the indexed sub-element of that arrayed object.
*/
int Trick::MemoryManager::ref_dim( REF2* R, V_DATA* V) {

    int jj;
    int item_size;

    if (R->ref_type != REF_ADDRESS) {
        message_publish(MSG_ERROR, "Memory Manager ERROR: Attempt to index into a non-address reference is bogus in ref_dim.\n") ;
        return (1);
    }

    R->num_index_left--;
    if (R->num_index_left < 0) {
        /* if we have too many dimensions, flag an error */
        message_publish(MSG_ERROR, "Memory Manager ERROR: Too many dimensions in ref_dim.\n") ;
        return (TRICK_PARAMETER_ARRAY_SIZE);
    }

    /* if current dimension is a fixed dimension */
    if (R->attr->index[R->num_index].size != 0) {

        /* Set the size of the element we are going to parse */
        if (R->attr->index[(R->attr->num_index - 1)].size == 0) {
            /* last dimension is a pointer, set item_size to sizeof pointer */
            item_size = sizeof(void *);
            R->pointer_present = 1 ;
        } else {
            /* all dimensions are fixed, set item_size to sizeof element */
            item_size = R->attr->size;
        }
        /* for fixed dimensions, we can check the validity of the index value */
        if (vval_int(V) >= R->attr->index[R->num_index].size || vval_int(V) < 0) {
            message_publish(MSG_ERROR, "Memory Manager ERROR: Array index out of bounds.\n") ;
            return (TRICK_PARAMETER_ARRAY_SIZE);
        }

        /* loop through all dimensions backwards, multiply the size of each dimension up to our current dimension
           (R->num_index) */
        for (jj = (R->attr->num_index - 1); jj > R->num_index; jj--) {
            if (R->attr->index[jj].size > 0) {
                item_size *= R->attr->index[jj].size;
            }
        }

    } else {

        R->pointer_present = 1 ;

        // Current dimension is a pointer.
        if (R->num_index == (R->attr->num_index - 1)) {
            item_size = R->attr->size;
        } else {
            item_size = sizeof(void *);
        }

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
            message_publish(MSG_ERROR, "Memory Manager ERROR: Reference (%s) address is NULL in ref_dim.\n", R->reference) ;
            return(TRICK_PARAMETER_ADDRESS_NULL) ;
        }
    }

    if ( R->create_add_path ) {
        
        ADDRESS_NODE * address_node ;

        if ( vval_int(V) > 0 ) {
            address_node = (ADDRESS_NODE *)DLL_GetAt(DLL_GetTailPosition(R->address_path), R->address_path) ;
            switch ( address_node->operator_ ) {
                case AO_ADDRESS:
                    address_node->operand.address = (void *)((char *)address_node->operand.address +  vval_int(V) * item_size) ;
                    break ;
                case AO_DEREFERENCE:
                    address_node = new ADDRESS_NODE ;
                    address_node->operator_ = AO_OFFSET ;
                    address_node->operand.offset = vval_int(V) * item_size ;
                    DLL_AddTail(address_node , R->address_path) ;
                    break ;
                case AO_OFFSET:
                    address_node->operand.offset += vval_int(V) * item_size ;
                    break ;
            }
        }
    }

    R->address =  (void*)((char*)R->address + vval_int(V) * item_size);
    R->num_index++;

    return (TRICK_NO_ERROR);
}
