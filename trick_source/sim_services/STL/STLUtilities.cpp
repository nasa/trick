
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "trick/STLUtilities.hh"
#include "trick/memorymanager_c_intf.h"

void Trick::delete_trick_stl(std::string object_name , unsigned int stl_id) {
    char var_declare[128] ;
    REF2 * items_ref ;

    sprintf(var_declare, "%s_%06d" , object_name.c_str(), stl_id) ;
    items_ref = ref_attributes(var_declare) ;
    if ( items_ref != NULL ) {
        TMM_delete_var_n(var_declare) ;
        ref_free(items_ref) ;
        free(items_ref) ;
    }
} ;

void Trick::delete_trick_map_stl( std::string obj_name , unsigned int stl_id ) {
    char var_declare[128] ;
    REF2 * items_ref ;

    sprintf(var_declare, "%s_%06d_keys" , obj_name.c_str(), stl_id) ;
    items_ref = ref_attributes(var_declare) ;
    if ( items_ref != NULL ) {
        TMM_delete_var_n(var_declare) ;
        ref_free(items_ref) ;
        free(items_ref) ;
    }
    sprintf(var_declare, "%s_%06d_data" , obj_name.c_str(), stl_id) ;
    items_ref = ref_attributes(var_declare) ;
    if ( items_ref != NULL ) {
        TMM_delete_var_n(var_declare) ;
        ref_free(items_ref) ;
        free(items_ref) ;
    }
}

