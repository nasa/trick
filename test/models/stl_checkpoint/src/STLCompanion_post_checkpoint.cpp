
#include <string>

#include "STLCompanion.hh"
#include "sim_services/CheckPointRestart/include/checkpoint_stl.hh"

/* All STLs are deleted through the same templated function delete_stl.  The 
   second and third arguments give the unique name to delete.  In
   this example the unique name is the concatenation of the object instance and the variable name.
   Currently the first argument is unused.
 */
int STLCompanion::post_checkpoint(STLCheckpoint * stlc, std::string object_name ) {

    delete_stl(stlc->my_double_map , object_name , std::string("my_double_map")) ;
    delete_stl(stlc->my_string_key_map , object_name , std::string("my_string_key_map")) ;
    delete_stl(stlc->my_string_data_map , object_name , std::string("my_string_data_map")) ;
    delete_stl(stlc->my_string_map , object_name , std::string("my_string_map")) ;

    delete_stl(stlc->my_int_multimap , object_name , std::string("my_int_multimap")) ;
    delete_stl(stlc->my_string_key_multimap , object_name , std::string("my_string_key_multimap")) ;
    delete_stl(stlc->my_string_data_multimap , object_name , std::string("my_string_data_multimap")) ;
    delete_stl(stlc->my_string_multimap , object_name , std::string("my_string_multimap")) ;

    delete_stl(stlc->my_double_vector , object_name , std::string("my_double_vector")) ;
    delete_stl(stlc->my_string_vector , object_name , std::string("my_string_vector")) ;

    delete_stl(stlc->my_short_list , object_name , std::string("my_short_list")) ;
    delete_stl(stlc->my_string_list , object_name , std::string("my_string_list")) ;

    delete_stl(stlc->my_float_deque , object_name , std::string("my_float_deque")) ;
    delete_stl(stlc->my_string_deque , object_name , std::string("my_string_deque")) ;

    delete_stl(stlc->my_int_set , object_name , std::string("my_int_set")) ;
    delete_stl(stlc->my_string_set , object_name , std::string("my_string_set")) ;

    delete_stl(stlc->my_long_multiset , object_name , std::string("my_long_multiset")) ;
    delete_stl(stlc->my_string_multiset , object_name , std::string("my_string_multiset")) ;

    delete_stl(stlc->my_uint_stack , object_name , std::string("my_uint_stack")) ;
    delete_stl(stlc->my_string_stack , object_name , std::string("my_string_stack")) ;

    delete_stl(stlc->my_int_queue , object_name , std::string("my_int_queue")) ;
    delete_stl(stlc->my_string_queue , object_name , std::string("my_string_queue")) ;

    delete_stl(stlc->my_int_priority_queue , object_name , std::string("my_int_priority_queue")) ;
    delete_stl(stlc->my_string_priority_queue , object_name , std::string("my_string_priority_queue")) ;

    delete_stl(stlc->my_int_pair , object_name , std::string("my_int_pair")) ;
    delete_stl(stlc->my_string_first_pair , object_name , std::string("my_string_first_pair")) ;
    delete_stl(stlc->my_string_second_pair , object_name , std::string("my_string_second_pair")) ;
    delete_stl(stlc->my_string_pair , object_name , std::string("my_stringpair")) ;

    return 0 ;
}

