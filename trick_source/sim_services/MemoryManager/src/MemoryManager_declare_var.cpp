#include <stdlib.h>
#include <iostream>

#include <sstream>
#include <dlfcn.h>
#include "sim_services/MemoryManager/include/MemoryManager.hh"
#include "sim_services/MemoryManager/include/ADefParseContext.hh"
#include "sim_services/Message/include/message_proto.h"
#include "sim_services/Message/include/message_type.h"

/**
 @page examples_declare_var Examples of declare_var

 Allocation of an anonymous singleton of type double:
 \code double *D = (double*)trick_MM->declare_var("double"); \endcode

 Allocation of an anonymous array of 3 doubles:
 \code double *D = (double*)trick_MM->declare_var("double[3]"); \endcode

 Allocation of an anonymous array of 3 pointers to double:
 \code double **D = (double**)trick_MM->declare_var("double*[3]"); \endcode

 Allocation of an anonymous 2 dimensional array of doubles:
 \code double (*A)[3][4] = (double(*)[3][4])trick_MM->declare_var("double[3][4]"); \endcode

 Allocation of a named singleton of type double:
 \code double *D = (double*)trick_MM->declare_var("double mydbl"); \endcode

 Allocation of a named array of 3 Pointers to double:
 \code double **D = (double**)trick_MM->declare_var("double* mydbl[3]"); \endcode

 Allocation of a named singleton of user-defined type "BAR":
 \code BAR *D = (BAR*)trick_MM->declare_var("BAR mydbl"); \endcode

 Allocation of a named 2 dimensional array of user-defined type "BAR" in namespace "FOO":
 \code FOO::BAR (*A)[3][4] = (FOO::BAR(*)[3][4])trick_MM->declare_var("FOO::BAR my_array[3][4]"); \endcode
 */

// PUBLIC MEMBER FUNCTION: void* Trick::MemoryManager::declare_var(TRICK_TYPE type,std::string user_type_name, int n_stars, std::string var_name, int n_cdims, int *cdims); 
void* Trick::MemoryManager::declare_var( TRICK_TYPE type,
                                        std::string user_type_name,
                                        int n_stars,
                                        std::string var_name,
                                        int n_cdims,
                                        int *cdims) {
    int ii;
    int size;
    char* allocation_name;
    int n_elems;
    Language language;
    void* address;
    ATTRIBUTES* sub_attr;
    ALLOC_INFO *new_alloc;
    VARIABLE_MAP::iterator variable_pos;

    if (debug_level > 1) {
        std::cout << __FUNCTION__ << ": Parameters: " << std::endl;
        std::cout << "           type = " << type << std::endl;
        std::cout << "           user_type_name = " << user_type_name << std::endl;
        std::cout << "           n_stars = " << n_stars << std::endl;
        std::cout << "           var_name = " << var_name << std::endl;
        std::cout << "           n_cdims = " << n_cdims << std::endl;
    }

    /** @par Design Details:
     This function is implemented using the following algorithm:
     */

    /** @li If this is a named allocation then determine whether the given variable name is already in use. */
    if (var_name != "") {
        pthread_mutex_lock(&mm_mutex);
        variable_pos = variable_map.find( var_name);
        if (variable_pos != variable_map.end()) {
            std::stringstream ss;
            ss << "Memory Manager ERROR: Variable \""<< var_name <<"\" already declared.\n";
            message_publish(MSG_ERROR, ss.str().c_str() );
            pthread_mutex_unlock(&mm_mutex);
            return ((void*)NULL);
        }
        pthread_mutex_unlock(&mm_mutex);
        allocation_name = strdup( var_name.c_str());
    } else {
        allocation_name = NULL;
    }

    /** @li Calculate the number of elements to be allocated. The number of elements is
            the product of the sizes of the constrained dimensions. */
    n_elems = 1;
    for (ii = 0; ii < n_cdims ; ii++ ) {
        n_elems = n_elems * cdims[ii];
    }
    if (n_elems == 0) {
        std::stringstream ss;
        ss << "Memory Manager ERROR: In the following declaration, ";
        ss << "one or more of the constrained dimensions is zero." ;
        ss << std::endl;

        // Print declaration.
        ss << make_decl_string( type, user_type_name, n_stars, var_name, n_cdims, cdims);
        ss << std::endl;
        message_publish(MSG_ERROR, ss.str().c_str() ) ;
        return ((void*)NULL);
    }

    /** @li From the TRICK_TYPE, user_type_name and the number of pointers (asterisks),
            determine the size and the attributes of an element. */
    if ( get_type_attributes(type, user_type_name, n_stars, sub_attr, size) != 0) {
        std::stringstream ss;
        ss << "Memory Manager ERROR: get_type_attributes failed for type: ";
        ss << trickTypeCharString(type, user_type_name.c_str());
        ss << std::endl;
        message_publish(MSG_ERROR, ss.str().c_str() ) ;

        return ((void*)NULL);
    }

    /** @li Allocate memory for the variable. */
    if ( (type == TRICK_STRUCTURED) &&
         (sub_attr->language == Language_CPP) &&
         (n_stars == 0 ) ) {

        if ((address = io_src_allocate_class( user_type_name.c_str(), n_elems)) == NULL) {
            std::stringstream ss;
            ss << "Memory Manager ERROR: io_src_allocate_class (";
            ss << user_type_name << "," << n_elems ;
            ss << ") failed to allocate any memory.";
            ss << std::endl;
            message_publish(MSG_ERROR, ss.str().c_str() ) ;

            return ((void*)NULL);
        }
        language = Language_CPP;

    } else if ((type == TRICK_STRING) && (n_stars == 0 ) ) {

        std::string *s = (std::string*)calloc(n_elems, sizeof(std::string));
        for (int ii=0 ; ii<n_elems ; ii++) {
            new( &s[ii]) std::string();
        }
        address = s;

        if (address == NULL) {
            return ((void*)NULL);
        }
        language = Language_CPP;
    } else {
        if ( (address = calloc( (size_t)n_elems, (size_t)size ) ) == NULL) {
            message_publish(MSG_ERROR, "Memory Manager ERROR: Out of memory.\n") ;
            return ((void*)NULL);
        }
        language = Language_C;
    }

    /** @li Allocate and populate an ALLOC_INFO record for the allocation. */
    if ((new_alloc = (ALLOC_INFO*)calloc(1, sizeof(ALLOC_INFO))) != NULL) {

        new_alloc->start = address;
        new_alloc->end = ( (char*)new_alloc->start) + (n_elems * size) - 1;
        new_alloc->name = allocation_name;
        new_alloc->stcl = TRICK_LOCAL;
        new_alloc->size = size;
        new_alloc->language = language;
        new_alloc->type = type;

        if ((type == TRICK_STRUCTURED) || (type == TRICK_ENUMERATED)) {
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

        new_alloc->id = alloc_info_map_counter++ ;

        /** @li Insert the <address, ALLOC_INFO> key-value pair into the alloc_info_map.*/
        pthread_mutex_lock(&mm_mutex);
        alloc_info_map[address] = new_alloc;

        /** @li If this is a named allocation: then insert the <variable-name, ALLOC_INFO>
            key-value pair into the variable map.*/
        if (new_alloc->name) {
            variable_map[new_alloc->name] = new_alloc;
        }
        pthread_mutex_unlock(&mm_mutex);
    } else {
        message_publish(MSG_ERROR, "Memory Manager ERROR: Out of memory.\n") ;
        return ((void*)NULL);
    }

    /** @li If debug is enabled, show what happened.*/
    if (debug_level) {
        int i;
        std::cout << std::endl;
        std::cout << "Allocation: " << new_alloc->num << " element(s) of type(" ;
        std::cout << trickTypeCharString(type, user_type_name.c_str()) ;
        for (i=0;i<n_stars;i++) std::cout << "*";
        std::cout << "), size(" << size << ") @ addr(" << address << ")." ;
        std::cout << std::endl << std::endl;
        std::cout.flush();
    }

    /** @li Return the address of the allocation. */
    return (address);
}

// PUBLIC MEMBER FUNCTION: void* Trick::MemoryManager::declare_var( const char *alloc_definition);
void* Trick::MemoryManager::declare_var( const char *alloc_definition) {

    void* address = NULL;

    Trick::ADefParseContext* context = NULL;
    std::stringstream alloc_decl_sstream;

    /** @par Design Details:
     This function is implemented using the following algorithm:
     */

    alloc_decl_sstream << alloc_definition;

    /** @li Create a parse context. */
    context = new Trick::ADefParseContext( &alloc_decl_sstream);

    /** @li Call ADEF_parse to parse the allocation definition. */
    if (context != NULL) {
        if ( ADEF_parse( context) == 0) {

            /** @li Call the general form of declare_var to perform the allocation. */
            address = declare_var( context->type,
                                  context->user_type_name,
                                  context->n_stars,
                                  context->var_name,
                                  context->n_cdims,
                                  context->cdims);

            /** @li Delete the parse context. */
            delete( context);
        } else {
            message_publish(MSG_ERROR, "Memory Manager: Invalid declaration (failed to parse): \"%s\".\n", alloc_definition) ;
        }
    }
    /** @li Return the address of the allocation. */
    return ( address);
}

// PUBLIC MEMBER FUNCTION: void* Trick::MemoryManager::declare_var( const char *element_definition, int n_elems);

void* Trick::MemoryManager::declare_var( const char *element_definition, int n_elems) {

    void* address = NULL;
    int cdims[8];
    int n_cdims;

    Trick::ADefParseContext* context = NULL;
    std::stringstream alloc_decl_sstream;

    /** We know that our array will be at least one dimensional and that dimension contains n_elems elements. */
    cdims[0] = n_elems;
    n_cdims = 1;

    alloc_decl_sstream << element_definition;

    /** @li Create a parse context. */
    context = new Trick::ADefParseContext( &alloc_decl_sstream);

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
            address = declare_var( context->type,
                                   context->user_type_name,
                                   context->n_stars,
                                   context->var_name,
                                   n_cdims,
                                   cdims);

            /** @li Delete the parse context. */
            delete( context);
        } else {
            message_publish(MSG_ERROR, "Memory Manager ERROR: declare_var( \"%s\",%d).\n", element_definition, n_elems) ;
        }
    }
    /** @li Return the address of the allocation. */
    return ( address);
}

// PUBLIC MEMBER FUNCTION: void* Trick::MemoryManager::declare_operatornew_var(std::string user_type_name, unsigned int alloc_size , unsigned int element_size );

void* Trick::MemoryManager::declare_operatornew_var( std::string user_type_name, unsigned int alloc_size , unsigned int element_size ) {
    TRICK_TYPE type = TRICK_STRUCTURED ;
    int size_ref = 1 ;
    void* address;
    int aligned;
    ATTRIBUTES* sub_attr;
    ALLOC_INFO *new_alloc;

    if (debug_level > 1) {
        std::cout << __FUNCTION__ << ": Parameters: " << std::endl;
        std::cout << "           user_type_name = " << user_type_name << std::endl;
        std::cout << "               alloc_size = " << alloc_size << std::endl;
        std::cout << "             element_size = " << element_size << std::endl;
    }

    /** @li From the TRICK_TYPE, user_type_name and the number of pointers (asterisks),
            determine the size and the attributes of an element. */
    if ( get_type_attributes(type, user_type_name, 0, sub_attr, size_ref ) != 0) {
        message_publish(MSG_ERROR, "Memory Manager ERROR: get_type_attributes failed for type: %d %s.\n",
                                    TRICK_STRUCTURED, user_type_name.c_str()) ;
        return ((void*)NULL);
    }

    if ( (address = calloc( 1, alloc_size ) ) == NULL) {
        message_publish(MSG_ERROR, "Memory Manager ERROR: Out of memory.\n") ;
        return ((void*)NULL);
    }

    aligned = alloc_size % element_size ;

    /** @li Allocate and populate an ALLOC_INFO record for the allocation. */
    if ((new_alloc = (ALLOC_INFO*)calloc(1, sizeof(ALLOC_INFO))) != NULL) {

        new_alloc->start = (char *)address + aligned ;
        new_alloc->end = ( (char*)new_alloc->start) + alloc_size - 1 - aligned ;
        new_alloc->name = NULL;
        new_alloc->stcl = TRICK_LOCAL;
        new_alloc->size = element_size ;
        new_alloc->sentinel_bytes = aligned ;
        new_alloc->language = Language_CPP ;
        new_alloc->type = TRICK_STRUCTURED ;

        new_alloc->user_type_name = strdup( user_type_name.c_str());

        new_alloc->attr = sub_attr;
        new_alloc->num = alloc_size / element_size ;

        new_alloc->num_index = 1;
        new_alloc->index[0] = new_alloc->num ;

        new_alloc->id = alloc_info_map_counter++ ;

        /** @li Insert the <address, ALLOC_INFO> key-value pair into the alloc_info_map.*/
        pthread_mutex_lock(&mm_mutex);
        alloc_info_map[address] = new_alloc;
        pthread_mutex_unlock(&mm_mutex);
    } else {
        message_publish(MSG_ERROR, "Memory Manager ERROR: Out of memory.\n") ;
        return ((void*)NULL);
    }

    /** @li If debug is enabled, show what happened.*/
    if (debug_level ) {
        std::cout << std::endl;
        std::cout << "Allocation: " << new_alloc->num << " element(s) of type(" ;
        std::cout << user_type_name ;
        std::cout << "), size(" << new_alloc->size << ") @ addr(" << address << ") sentinel_bytes = " << new_alloc->sentinel_bytes ;
        std::cout << std::endl << std::endl;
        std::cout.flush();
    }

    /** @li Return the address of the allocation. */
    return (address);
}

// PUBLIC MEMBER FUNCTION: size_t Trick::MemoryManager::sizeof_type( const char* var_definition);

size_t Trick::MemoryManager::sizeof_type( const char* var_definition) {

    int ii;
    int size = 0 ;
    int n_elems = 0 ;
    ATTRIBUTES* sub_attr;

    Trick::ADefParseContext* context = NULL;
    std::stringstream alloc_decl_sstream;

    alloc_decl_sstream << var_definition;
    context = new Trick::ADefParseContext( &alloc_decl_sstream);
    if (context != NULL) {
        if ( ADEF_parse( context) == 0) {

            get_type_attributes(context->type, context->user_type_name, context->n_stars, sub_attr, size);

            n_elems = 1;
            for (ii=0; ii<context->n_cdims ; ii++) {
                n_elems *= context->cdims[ii];
            }
        } else {
            message_publish(MSG_ERROR, "Memory Manager: Invalid variable definition \"%s\" in sizeof_type.\n", var_definition) ;
        }
        delete( context);
    }

    return (size_t)(n_elems * size);
}
