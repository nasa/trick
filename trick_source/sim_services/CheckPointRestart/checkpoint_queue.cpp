
#include "trick/checkpoint_queue.hh"

int checkpoint_stl(std::queue<std::string> & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;
    int status ;

    char ** items = NULL ;
    std::queue<std::string> temp_queue ;

    cont_size = in_stl.size() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    if ( cont_size > 0 ) {

        sprintf(var_declare, "%s %s_%s[%d]" ,
         abi::__cxa_demangle(typeid(*items).name(), 0, 0, &status ), object_name.c_str(), var_name.c_str(), cont_size) ;
        items = (char **)TMM_declare_var_s(var_declare) ;
        //message_publish(1, "CHECKPOINT_STL_STACK with %s\n", var_declare) ;

        temp_queue = in_stl ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            items[ii] = (char *)((temp_queue.front()).c_str()) ;
            temp_queue.pop() ;
        }
    }

    return 0 ;
}

int checkpoint_stl(std::priority_queue<std::string> & in_stl , std::string object_name , std::string var_name ) {

    unsigned int ii ;
    unsigned int cont_size ;
    char var_declare[128] ;
    int status ;

    char ** items = NULL ;
    std::priority_queue<std::string> temp_queue ;

    cont_size = in_stl.size() ;
    std::replace_if(object_name.begin(), object_name.end(), std::ptr_fun<int,int>(&std::ispunct), '_');

    if ( cont_size > 0 ) {

        sprintf(var_declare, "%s %s_%s[%d]" ,
         abi::__cxa_demangle(typeid(*items).name(), 0, 0, &status ), object_name.c_str(), var_name.c_str(), cont_size) ;
        items = (char **)TMM_declare_var_s(var_declare) ;
        //message_publish(1, "CHECKPOINT_STL_STACK with %s\n", var_declare) ;

        temp_queue = in_stl ;

        for ( ii = 0 ; ii < cont_size ; ii++ ) {
            items[ii] = (char *)((temp_queue.top()).c_str()) ;
            temp_queue.pop() ;
        }
    }

    return 0 ;
}

int restore_stl(std::queue<std::string> & in_stl , std::string object_name , std::string var_name ) {
    return restore_queue_stl_string(in_stl , object_name , var_name) ;
}

int restore_stl(std::priority_queue<std::string> & in_stl , std::string object_name , std::string var_name ) {
    return restore_queue_stl_string(in_stl , object_name , var_name) ;
}


