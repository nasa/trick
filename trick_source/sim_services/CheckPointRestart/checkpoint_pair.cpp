
#include <string>
#include <utility>

#include "trick/checkpoint_pair.hh"

int checkpoint_stl(std::pair<std::string, std::string> & in_stl , std::string object_name , std::string var_name ) {

    char var_declare[128] ;
    char ** first ;
    char ** second ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    sprintf(var_declare, "char * %s_%s_first[1]", object_name.c_str(), var_name.c_str()) ;
    first = (char **)TMM_declare_var_s(var_declare) ;
    TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name + "_first").c_str()) ;

    sprintf(var_declare, "char * %s_%s_second[1]" , object_name.c_str(), var_name.c_str()) ;
    second = (char **)TMM_declare_var_s(var_declare) ;
    TMM_add_checkpoint_alloc_dependency(std::string(object_name + "_" + var_name + "_second").c_str()) ;

    first[0] = (char *)(in_stl.first.c_str()) ;
    second[0] = (char *)(in_stl.second.c_str()) ;

    return 0 ;
}

int restore_stl(std::pair<std::string, std::string> & in_stl , std::string object_name , std::string var_name ) {

    REF2 * first_ref ;
    REF2 * second_ref ;

    char ** first ;
    char ** second ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');
    //message_publish(1, "RESTORE_STL_queue %s_%s\n", object_name.c_str() , var_name.c_str()) ;

    first_ref = ref_attributes((char *)(object_name + std::string("_") + var_name + std::string("_first")).c_str()) ;
    second_ref = ref_attributes((char *)(object_name + std::string("_") + var_name + std::string("_second")).c_str()) ;

    if ( first_ref != NULL && second_ref != NULL ) {
        first = (char **)first_ref->address ;
        second = (char **)second_ref->address ;

        in_stl.first = std::string(first[0]) ;
        in_stl.second = std::string(second[0]) ;

        delete_stl( in_stl , object_name , var_name ) ;
    } 

    return 0 ;
}

