
#include "sim_services/MemoryManager/include/reference.h"
#include "sim_services/MemoryManager/include/memorymanager_c_intf.h"

void * follow_address_path(REF2 * R) {

    DLLPOS list_pos ;
    ADDRESS_NODE * address_node ;
    void * address ;

    address = NULL ;

    list_pos = DLL_GetHeadPosition(R->address_path) ;
    while ( list_pos != NULL ) {
        address_node = (ADDRESS_NODE *)DLL_GetNext(&list_pos, R->address_path) ;
        switch ( address_node->operator_ ) {
            case AO_ADDRESS:
                address = address_node->operand.address  ;
                break ;
            case AO_DEREFERENCE:
                address = *(char **)address ;
                break ;
            case AO_OFFSET:
                address = (void *)((char *)address + address_node->operand.offset) ;
                break ;
        }
    }

    return(address) ;
}



