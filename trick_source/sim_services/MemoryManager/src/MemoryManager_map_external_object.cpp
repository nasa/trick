#include <sstream>
#include <stdlib.h>

#include "sim_services/MemoryManager/include/MemoryManager.hh"
#include "sim_services/MemoryManager/include/ADefParseContext.hh"
#include "sim_services/Message/include/message_proto.h"
#include "sim_services/Message/include/message_type.h"

/**
 @par Description:
 declare_extern_var describes an externally allocated memory object to the memory manager.
 */

// MEMBER FUNCTION
void* Trick::MemoryManager::
      declare_extern_var( void* address,
                           TRICK_TYPE type,
                           std::string user_type_name,
                           int n_stars,
                           std::string var_name,
                           int n_cdims,
                           int *cdims) {

    int ii;
    int size;
    char* allocation_name;
    int n_elems;
    ATTRIBUTES* sub_attr;
    ALLOC_INFO *new_alloc;
    VARIABLE_MAP::iterator variable_pos;

    /** @par Design Details:
     This function is implemented using the following algorithm:
     */

    /** @li Validate Parameters. The address can't be NULL.*/
    if (address == NULL) {
        message_publish(MSG_ERROR, "Memory Manager ERROR: declare_extern_var called with NULL address.\n") ;
        return ((void*)NULL);
    }

    /** @li Determine whether the given variable name is already in use. */
    if (var_name != "") {
        pthread_mutex_lock(&mm_mutex);
        variable_pos = variable_map.find( var_name);
        if (variable_pos != variable_map.end()) {
            message_publish(MSG_ERROR, "Memory Manager ERROR: Variable \"%s\" already declared.\n", var_name.c_str()) ;
            pthread_mutex_unlock(&mm_mutex);
            return ((void*)NULL);
        }
        pthread_mutex_unlock(&mm_mutex);
        allocation_name = strdup( var_name.c_str());
    } else {
        allocation_name = NULL;
    }


    /** @li Calculate the number of elements in this external variable. This is
            the product of the sizes of the constrained dimensions. */
    n_elems = 1;
    for (ii = 0; ii < n_cdims ; ii++ ) {
        n_elems = n_elems * cdims[ii];
    }
    if (n_elems == 0) {
        // FIXME: This Error message needs to improved.
        message_publish(MSG_ERROR, "Memory Manager ERROR: One or more of the constrained dimensions is zero.\n") ;
        return ((void*)NULL);
    }

    /** @li From the TRICK_TYPE, user_type_name and the number of pointers (asterisks),
            determine the size and the attributes of an element. */
    if ( get_type_attributes(type, user_type_name, n_stars, sub_attr, size) != 0) {
        message_publish(MSG_ERROR, "Memory Manager ERROR: get_type_attributes failed for type %d %s.\n", type, user_type_name.c_str()) ;
        return ((void*)NULL);
    }

    /** @li Allocate and populate an ALLOC_INFO record for the external allocation
        (the thingy pointed to by @b address).
     */
    if ((new_alloc = (ALLOC_INFO*)calloc(1, sizeof(ALLOC_INFO))) != NULL) {

        new_alloc->start = (void *) address;
        new_alloc->end = ((char*)new_alloc->start) + (n_elems * size) - 1;
        new_alloc->name = allocation_name;
        new_alloc->stcl = TRICK_EXTERN;
        new_alloc->alloc_type = TRICK_ALLOC_OTHER;
        new_alloc->size = size;
        if ( sub_attr != NULL ) {
            new_alloc->language = sub_attr->language ;
        } else {
            new_alloc->language = Language_C ;
        }
        new_alloc->type = type;

        if ((type == TRICK_STRUCTURED) || (type == TRICK_ENUMERATED) || (type == TRICK_OPAQUE_TYPE)) {
            new_alloc->user_type_name = strdup( user_type_name.c_str());
        } else {
            new_alloc->user_type_name = NULL ;
        }

        new_alloc->attr = sub_attr;
        new_alloc->num = n_elems;

        new_alloc->num_index = 0;
        for (ii = 0; ii < n_cdims ; ii++ ) {
            new_alloc->index[new_alloc->num_index] = cdims[ii];
            new_alloc->num_index ++ ;
        }
        for (ii = 0 ; ii < n_stars ; ii++) {
            new_alloc->index[new_alloc->num_index] = 0;
            new_alloc->num_index ++ ;
        }

        new_alloc->id = extern_alloc_info_map_counter++ ;

        /** @li Insert the <address, ALLOC_INFO> key-value pair into the alloc_info_map.*/
        pthread_mutex_lock(&mm_mutex);
        alloc_info_map[address] = new_alloc;

        /** @li Insert the <variable-name, ALLOC_INFO> key-value pair into the variable map. */
        if (new_alloc->name) {
            variable_map[new_alloc->name] = new_alloc;
        }
        pthread_mutex_unlock(&mm_mutex);
    } else {
        message_publish(MSG_ERROR, "Memory Manager ERROR: Out of memory.\n") ;
        return ((void*)NULL);
    }

    if (debug_level) {
        int i;
        std::cout << std::endl;
        std::cout << "Extern declaration: " << new_alloc->num << " element(s) of type(" ;
        std::cout << trickTypeCharString(type, user_type_name.c_str()) ;
        for (i=0;i<n_stars;i++) {
            std::cout << "*";
        }
        std::cout << "), size(" << size << ") @ addr(" << address << ")." ;
        std::cout << std::endl << std::endl;
        std::cout.flush();
    }

    /** @li Return the address of the allocation. */
    return (address);
}

// MEMBER FUNCTION
void* Trick::MemoryManager::
      declare_extern_var(void*       address,
                          const char* alloc_definition) {

    void* res_address = NULL;

    ADefParseContext* context = NULL;
    std::stringstream alloc_decl_sstream;

    /** @par Design Details:
     This function is implemented using the following algorithm:
     */

    alloc_decl_sstream << alloc_definition;

    /** @li Create a parse context. */
    context = new ADefParseContext( &alloc_decl_sstream);

    /** @li Call ADEF_parse to parse the declaration. */
    if (context != NULL) {
        if ( ADEF_parse( context) == 0) {

            /** @li Call the general form of declare_extern_var. */
            res_address = declare_extern_var( address,
                                               context->type,
                                               context->user_type_name,
                                               context->n_stars,
                                               context->var_name,
                                               context->n_cdims,
                                               context->cdims);

            /** @li Delete the parse context. */
            delete( context);
        } else {
            message_publish(MSG_ERROR, "Memory Manager: Invalid declaration \"%s\".\n", alloc_definition) ;
        }
    }
    /** @li Return the address. */
    return ( res_address);
}

// MEMBER FUNCTION
void* Trick::MemoryManager::
      declare_extern_var( void*       address,
                          const char* element_definition,
                          int         n_elems) {

    void* res_address = NULL;
    int cdims[8];
    int n_cdims;
 
    ADefParseContext* context = NULL;
    std::stringstream alloc_decl_sstream;

    /** We know that our array will be at least one dimensional and that dimension contains n_elems elements. */
    cdims[0] = n_elems;
    n_cdims = 1;

    alloc_decl_sstream << element_definition;

    /** @li Create a parse context. */
    context = new ADefParseContext( &alloc_decl_sstream);

    if (context != NULL) {

    /** @li Parse the allocation definition and ensure that the dimension is at least one less than the maximum of 8. */
        if (( ADEF_parse( context) == 0) && (context->n_cdims < 8) ){
            int ii;

            /** @li Add the dimensions of the element definition. */
            for (ii=0 ; ii < context->n_cdims ; ii++) {
                cdims[ii+1] = context->cdims[ii];
                n_cdims ++;
            }

            /** @li Call the general form of declare_var to perform the allocation. */
            res_address = declare_extern_var( address,
                                              context->type,
                                              context->user_type_name,
                                              context->n_stars,
                                              context->var_name,
                                              n_cdims,
                                              cdims);

            /** @li Delete the parse context. */
            delete( context);
        } else {
            message_publish(MSG_ERROR, "Memory Manager ERROR: declare_extern_var( \"%s\",%d).\n", element_definition, n_elems) ;
        }
    }
    /** @li Return the address of the allocation. */
    return ( res_address);
}
