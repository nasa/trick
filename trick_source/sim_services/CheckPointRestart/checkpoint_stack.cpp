
#include "trick/checkpoint_stack.hh"

int checkpoint_stl(std::stack<std::string> & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;
    int status ;

    char ** items = NULL ;
    std::stack<std::string> temp_stack ;

    cont_size = in_stl.size() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    if ( cont_size > 0 ) {

        sprintf(var_declare, "%s %s_%s[%d]" ,
         abi::__cxa_demangle(typeid(*items).name(), 0, 0, &status ), object_name.c_str(), var_name.c_str(), cont_size) ;
        items = (char **)TMM_declare_var_s(var_declare) ;
        //message_publish(1, "CHECKPOINT_STL_STACK with %s\n", var_declare) ;

        temp_stack = in_stl ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            items[ii] = (char *)((temp_stack.top()).c_str()) ;
            temp_stack.pop() ;
        }
    }

    return 0 ;
}

int restore_stl(std::stack<std::string> & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;

    REF2 * items_ref ;
    char ** items ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    //message_publish(1, "RESTORE_STL_STACK %s_%s\n", object_name.c_str() , var_name.c_str()) ;

    cont_size = in_stl.size() ;
    for ( ii = 0 ; ii < cont_size ; ii++ ) {
        in_stl.pop() ;
    }

    items_ref = ref_attributes((char *)(object_name + std::string("_") + var_name).c_str()) ;

    if ( items_ref != NULL ) {
        items = (char **)items_ref->address ;
        cont_size = get_size((char *)items) ;

        for ( ii = cont_size - 1 ; ii < cont_size ; ii-- ) {
            in_stl.push( items[ii] ) ;
        }
        delete_stl( in_stl , object_name , var_name ) ;
    }

    return 0 ;
}

