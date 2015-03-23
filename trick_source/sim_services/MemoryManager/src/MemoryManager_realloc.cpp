#include "sim_services/MemoryManager/include/MemoryManager.hh"
#include "sim_services/Message/include/message_proto.h"
#include "sim_services/Message/include/message_type.h"
#include <dlfcn.h>
#include <stdlib.h>

// PRIVATE MEMBER FUNCTION
void Trick::MemoryManager::recursive_array_copy( void *s_base,
                                                 void *d_base,
                                                 int *s_cdims,
                                                 int *d_cdims,
                                                 size_t size,
                                                 int n,
                                                 int cur,
                                                 int s_off,
                                                 int d_off) {
    char *s_addr, *d_addr;
    int ii;

    if (cur == n) {
        s_addr = (char*)s_base + s_off * size;
        d_addr = (char*)d_base + d_off * size;
        memcpy( d_addr, s_addr, size);
    } else {

        for (ii=0 ; (ii<s_cdims[cur])&&(ii<d_cdims[cur]) ; ii++ ) {
              recursive_array_copy( s_base,
                                    d_base,
                                    s_cdims,
                                    d_cdims,
                                    size,
                                    n,
                                    cur+1,
                                    s_off * s_cdims[cur] + ii,
                                    d_off * d_cdims[cur] + ii);
        }
    }
}


// PRIVATE MEMBER FUNCTION
void Trick::MemoryManager::array_copy( void *s_base,
                                       void *d_base,
                                       int *s_cdims,
                                       int *d_cdims,
                                       size_t size,
                                       int n) {

   recursive_array_copy( s_base, d_base, s_cdims, d_cdims, size, n, 0, 0, 0);
}


// PUBLIC MEMBER FUNCTION
void* Trick::MemoryManager::resize_array( void *address, int n_cdims, int *cdims) {

    ALLOC_INFO *alloc_info;
    int ii;
    int pre_n_cdims;
    int new_n_elems;
    int n_stars;
    void *new_address;

    /** @li Find the ALLOC_INFO record for the object at the given address.*/
    pthread_mutex_lock(&mm_mutex);
    alloc_info = get_alloc_info_at( address);

    if (alloc_info == NULL) {
        message_publish(MSG_ERROR, "Memory Manager ERROR: Address passed to resize_array is NULL.\n") ;
        pthread_mutex_unlock(&mm_mutex);
        return ( (void*)NULL);
    }

    if (alloc_info->stcl != TRICK_LOCAL) {
        message_publish(MSG_ERROR, "Memory Manager ERROR: Cannot resize the array variable at address %p "
                                   "because it did not allocate it.\n", address) ;
        pthread_mutex_unlock(&mm_mutex);
        return ( (void*)NULL);
    }

    if (alloc_info->type == TRICK_STRING) {
        message_publish(MSG_ERROR, "Memory Manager resize_array ERROR: at address %p."
                                   " Arrays of STL strings are not supported."
                                   " Consider using a vector of strings.\n", address) ;
        pthread_mutex_unlock(&mm_mutex);
        return ( (void*)NULL);
    }

    /** @li Determine the number of constrained dimensions in the existing allocation.*/
    pre_n_cdims = 0;
    for (ii=0 ; ii<alloc_info->num_index ; ii++) { 
       if (alloc_info->index[ii]!=0) {
           pre_n_cdims++;
       }
    }
    n_stars = alloc_info->num_index - pre_n_cdims;
    pthread_mutex_unlock(&mm_mutex);

    /** @li Verify that the number of constrained dimensions in the existing allocation
        are the same as those in the proposed re-allocation.*/
    if (n_cdims != pre_n_cdims) {
        if (pre_n_cdims == 0) {
            message_publish(MSG_ERROR, "Memory Manager resize_array ERROR: The object at address %p is not an array.\n"
                                       "If you want to be able to resize it then\n"
                                       "declare it as an array (of one) in the first place.\n", address) ;
        } else {
            message_publish(MSG_ERROR, "Memory Manager resize_array ERROR: The number of dimensions specified, \"%d\",\n"
                                       "doesn't match the number of (non-pointer) dimensions of the object, \"%d\",\n"
                                       "at address %p\n.", n_cdims, pre_n_cdims, address) ;
        }
        return ( (void*)NULL);
    } 

    /** @li Calculate the new number of elements to be allocated. */
    new_n_elems = 1;
    for (ii = 0; ii < n_cdims ; ii++ ) {
        new_n_elems = new_n_elems * cdims[ii];
    }
    if (new_n_elems == 0) {
        message_publish(MSG_ERROR, "Memory Manager ERROR: One or more of the constrained dimensions is zero.\n") ;
        return ((void*)NULL);
    }

    /** @li Allocate new memory. */
    pthread_mutex_lock(&mm_mutex);
    if ( (alloc_info->type == TRICK_STRUCTURED) &&
         (alloc_info->language == Language_CPP) &&
         (alloc_info->num_index == n_cdims) ) {

        if ((new_address = io_src_allocate_class( alloc_info->user_type_name, new_n_elems)) == NULL) {
            message_publish(MSG_ERROR, "Memory Manager ERROR: io_src_allocate_class(%s,%d) failed to allocate any memory.\n",
                      alloc_info->user_type_name, new_n_elems) ;
            pthread_mutex_unlock(&mm_mutex);
            return ((void*)NULL);
        }  
    } else if ((alloc_info->type == TRICK_STRING) && (n_stars == 0 ) ) {
        new_address = new (std::nothrow) std::string[new_n_elems];
        if (new_address == NULL) {
            pthread_mutex_unlock(&mm_mutex);
            return ((void*)NULL);
        }
    } else {
        if ( (new_address = calloc( (size_t)new_n_elems, (size_t)alloc_info->size ) ) == NULL) {
            message_publish(MSG_ERROR, "Memory Manager ERROR: Out of memory.\n") ;
            pthread_mutex_unlock(&mm_mutex);
            return ((void*)NULL);
        }
    }

    /** @li Copy the data from the previous array to the new array, constrained by the common extents.*/
    array_copy( alloc_info->start,
                new_address,
                alloc_info->index,
                cdims,
                alloc_info->size,
                n_cdims);

    /** @li Delete the previous memory allocation. */
    free( address);

    // Remove the old <address, ALLOC_INFO*> key-value pair from the alloc_info_map.
    alloc_info_map.erase( address);

    /** @li Update the ALLOC_INFO record with new start and end addresses, with
            new extents and with the new number of elements.*/
    alloc_info->start = new_address;
    alloc_info->end   = ( (char*)alloc_info->start) + (new_n_elems * alloc_info->size) - 1;
    for (ii=0 ; ii < n_cdims ; ii++) {
        alloc_info->index[ii] = cdims[ii];
    }
    alloc_info->num = new_n_elems;

    /** @li Insert the new <address, ALLOC_INFO> key-value pair into the alloc_info_map.*/
    alloc_info_map[alloc_info->start] = alloc_info;
    pthread_mutex_unlock(&mm_mutex);

    /** @li If debug is enabled, show what happened.*/
    if (debug_level) {
        int i;
        std::cout << std::endl;
        std::cout << "Re-allocation: " << alloc_info->num << " element(s) of type(" ;
        std::cout << trickTypeCharString( alloc_info->type, alloc_info->user_type_name ) ;
        for (i=0;i<n_stars;i++) std::cout << "*";
        std::cout << "," << alloc_info->type << "), size(" << alloc_info->size << ") @ addr(" << alloc_info->start << ")." ;
        std::cout << std::endl << std::endl;
        std::cout.flush();
    }
    return (new_address);
}


// PUBLIC MEMBER FUNCTION
void* Trick::MemoryManager::resize_array( void* address, int num) {
    int dimension = num;

    return( resize_array( address, 1, &dimension));
}


// PUBLIC MEMBER FUNCTION
void* Trick::MemoryManager::resize_array( const char* name, int n_cdims, int *cdims) {

    VARIABLE_MAP::iterator pos;
    ALLOC_INFO *alloc_info;

    pthread_mutex_lock(&mm_mutex);
    pos = variable_map.find( name);

    if (pos != variable_map.end()) {
        alloc_info = pos->second;
        pthread_mutex_unlock(&mm_mutex);
        return( resize_array( alloc_info->start, n_cdims, cdims));
    } else {
        message_publish(MSG_ERROR, "Memory Manager ERROR: Cannot resize variable \"%s\" because it doesn't exist.\n", name) ;
    }
    pthread_mutex_unlock(&mm_mutex);
    return NULL ;
}


// PUBLIC MEMBER FUNCTION
void* Trick::MemoryManager::resize_array( const char* name, int num) {
    int dimension = num;

    return( resize_array( name, 1, &dimension));
}
