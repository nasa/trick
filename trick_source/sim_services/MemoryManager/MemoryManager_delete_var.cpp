#include <stdlib.h>
#include <iostream>

#include <sstream>
#include <dlfcn.h>
#include "trick/MemoryManager.hh"

// MEMBER FUNCTION
int Trick::MemoryManager::delete_var(void* address, bool destroy ) {

    if (address == 0) {
        if (debug_level) {
            std::stringstream message;
            message << "Cannot delete memory at NULL.";
            emitWarning(message.str());
        }
        return 1;
    }

    // Find the allocation record for this address.
    ALLOC_INFO * alloc_info = get_alloc_info_at( address);

    if ((alloc_info != NULL)) {
        /*
           Remove the allocation from the map first so we don't have trouble
           if io_src_destruct_class or io_src_delete_class calls delete_var recursively.
         */
        // BEGIN PROTECTION of the alloc_info_map.
        pthread_mutex_lock(&mm_mutex);
        alloc_info_map.erase( address);
        // END PROTECTION of the alloc_info_map.
        pthread_mutex_unlock(&mm_mutex);

        /* Verify that it's OK to free the memory at the given address.
           It's OK if an allocation record exists for this address and the
           MemoryManager allocated it.
         */
        if ( alloc_info->stcl == TRICK_LOCAL ) {
            if ( alloc_info->alloc_type == TRICK_ALLOC_MALLOC ) {
                if (destroy) {
                    io_src_destruct_class( alloc_info );
                }
                free( address);
            } else if ( alloc_info->alloc_type == TRICK_ALLOC_NEW ) {
                io_src_delete_class( alloc_info );
            }
        }

        if (debug_level) {
            std::cout << "Deleted allocation at address " << alloc_info->start << "." << std::endl;
            std::cout.flush();
        }

        if (alloc_info->name ) {
            pthread_mutex_lock(&mm_mutex);
            variable_map.erase( alloc_info->name);
            pthread_mutex_unlock(&mm_mutex);
            free(alloc_info->name);
        }

        if ( alloc_info->user_type_name ) {
            free(alloc_info->user_type_name);
        }

        // Delete the alloc_info record.
        free(alloc_info);

    } else {
        std::stringstream message;
        message << "The MemoryManager cannot delete memory at address ["
                << address << "] because it has no record of it.";
        emitWarning(message.str());
        return 1 ;
    }

    return 0;
}

// MEMBER FUNCTION
int Trick::MemoryManager::delete_var( const char* name) {

    VARIABLE_MAP::iterator pos;
    ALLOC_INFO *alloc_info;

    pthread_mutex_lock(&mm_mutex);
    pos = variable_map.find( name);

    if (pos != variable_map.end()) {
        alloc_info = pos->second;

        pthread_mutex_unlock(&mm_mutex);
        return( delete_var( alloc_info->start));
    } else {
        std::stringstream message;
        message << "Cannot delete variable \"" << name
                << "\" because it doesn't exist." ;
        emitError(message.str());
    }
    pthread_mutex_unlock(&mm_mutex);
    return 1;
}

// MEMBER FUNCTION
int Trick::MemoryManager::delete_extern_var( void* address) {
    // delete_var will remove the external variable from the allocation map
    return delete_var(address) ;
}

// MEMBER FUNCTION
int Trick::MemoryManager::delete_extern_var( const char* name) {
    // delete_var will remove the external variable from the allocation map
    return delete_var(name) ;
}

