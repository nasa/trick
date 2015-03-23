#include <fstream>
#include <string.h>
#include <stdlib.h>  // free()
#include <algorithm> // std::sort()
#include "sim_services/MemoryManager/include/MemoryManager.hh"
#include "sim_services/Message/include/message_proto.h"
#include "sim_services/Message/include/message_type.h"

// GreenHills stuff
#if ( __ghs )
#include "ghs_stubs.h"
#endif

// MEMBER FUNCTION
void Trick::MemoryManager::write_checkpoint( std::ostream& out_s, std::vector<ALLOC_INFO*>& dependencies) {

    ALLOC_INFO_MAP::iterator pos;
    ALLOC_INFO* alloc_info;
    char name[256];
    int local_anon_var_number;
    int extern_anon_var_number;

    // 1) Generate declaration statements for each the allocations that we are managing.
    out_s << "// Variable Declarations." << std::endl;
    out_s.flush();

    local_anon_var_number = 0;
    extern_anon_var_number = 0;

    int n_depends = dependencies.size();
    for (int ii = 0 ; ii < n_depends ; ii ++) {
        alloc_info = dependencies[ii];

        /** Generate temporary names for anonymous variables. */
        if (alloc_info->name == NULL) {
            if ( alloc_info->stcl == TRICK_LOCAL) {
                sprintf( name, "%s%d", local_anon_var_prefix, local_anon_var_number++);
                alloc_info->name = strdup( name);
                currentCheckPointAgent->write_decl( out_s, alloc_info);
            } else if (alloc_info->stcl == TRICK_EXTERN) {
                sprintf( name, "%s%d", extern_anon_var_prefix, extern_anon_var_number++);
                alloc_info->name = strdup( name);
                /** @b NOTE: We should not write declarations for external
                    anonymous variables, because we should not reload them.*/
            } else {
                message_publish(MSG_ERROR, "Memory Manager INTERNAL ERROR: Allocation storage class is messed up.\n") ;
            }
        } else {
            currentCheckPointAgent->write_decl( out_s, alloc_info);
        }
    }

    // Write a "clear_all_vars" command.
    if (reduced_checkpoint) {
        out_s << std::endl << std::endl << "// Clear all allocations to 0." << std::endl;
        out_s << "clear_all_vars();" << std::endl;
    }

    // 2) Dump the contents of each of the dynamic and mapped allocations.
    out_s << std::endl << std::endl << "// Variable Assignments." << std::endl;
    out_s.flush();

    for (int ii = 0 ; ii < n_depends ; ii ++) {
        alloc_info = dependencies[ii];
        write_var( out_s, alloc_info);
        out_s << std::endl;
    }

    // Free all of the temporary names that were created for the checkpoint.

    for (int ii = 0 ; ii < n_depends ; ii ++) {
        alloc_info = dependencies[ii];
        // If the temporary-variable prefix occurs at the beginning of the name ...
        if ((alloc_info->name != NULL) &&
            (( strstr( alloc_info->name, local_anon_var_prefix ) == alloc_info->name ) ||
             ( strstr( alloc_info->name, extern_anon_var_prefix) == alloc_info->name ))) {
                free( alloc_info->name);
                alloc_info->name = NULL;
        }
    }
}

// Local sort function used in write_checkpoint.
static bool alloc_info_id_compare(ALLOC_INFO * lhs, ALLOC_INFO * rhs) { return ( lhs->id < rhs->id ) ; }

// MEMBER FUNCTION
void Trick::MemoryManager::write_checkpoint( std::ostream& out_s) {

    ALLOC_INFO_MAP::iterator pos;
    ALLOC_INFO* alloc_info;
    std::vector<ALLOC_INFO*> dependencies;

    pthread_mutex_lock(&mm_mutex);
    for ( pos=alloc_info_map.begin() ; pos!=alloc_info_map.end() ; pos++ ) {
        alloc_info = pos->second;
        dependencies.push_back(alloc_info);
    }

    // Sort the dependencies by ALLOC_INFO.id.
    std::sort( dependencies.begin() , dependencies.end() , alloc_info_id_compare) ;
    pthread_mutex_unlock(&mm_mutex);
    write_checkpoint( out_s, dependencies);

}

// MEMBER FUNCTION
void Trick::MemoryManager::write_checkpoint(const char* filename) {

   std::ofstream outfile( filename, std::ios::out);

    if (outfile.is_open()) {
        write_checkpoint( outfile);
    } else {
        message_publish(MSG_ERROR, "Memory Manager ERROR: Couldn't open \"%s\".\n", filename) ;
    }
}

// MEMBER FUNCTION
void Trick::MemoryManager::write_checkpoint( std::ostream& out_s, const char* var_name) {

    std::vector<ALLOC_INFO*> dependencies;

    pthread_mutex_lock(&mm_mutex);
    get_alloc_deps_in_allocation( dependencies, var_name);
    pthread_mutex_unlock(&mm_mutex);

    write_checkpoint( out_s, dependencies);
}

// MEMBER FUNCTION
void Trick::MemoryManager::write_checkpoint(const char* filename, const char* var_name) {

    std::vector<ALLOC_INFO*> dependencies;
    std::ofstream out_s( filename, std::ios::out);

    if (out_s.is_open()) {
        write_checkpoint( out_s, var_name);
    } else {
        std::cerr << "ERROR: Couldn't open \""<< filename <<"\"." << std::endl;
        std::cerr.flush();
    }
}

// MEMBER FUNCTION
void Trick::MemoryManager::write_checkpoint( std::ostream& out_s, std::vector<const char*>& var_name_list) {

    std::vector<ALLOC_INFO*> dependencies;
    const char* var_name;
    int n_names;

    n_names = var_name_list.size();
    for (int ii=0; ii< n_names; ii++) {
        var_name = var_name_list[ii];
        pthread_mutex_lock(&mm_mutex);
        get_alloc_deps_in_allocation(dependencies, var_name);
        pthread_mutex_unlock(&mm_mutex);
    }

    write_checkpoint( out_s, dependencies);
}

// MEMBER FUNCTION
void Trick::MemoryManager::write_checkpoint(const char* filename, std::vector<const char*>& var_name_list) {

    std::ofstream out_s( filename, std::ios::out);

    if (out_s.is_open()) {
        write_checkpoint( out_s, var_name_list);
    } else {
        std::cerr << "ERROR: Couldn't open \""<< filename <<"\"." << std::endl;
        std::cerr.flush();
    }
}
