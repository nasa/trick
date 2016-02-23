
#include <string>

#include "STLCompanion.hh"
#include "sim_services/CheckPointRestart/include/checkpoint_stl.hh"

/* All STLs are restored through the same templated function restore_stl.  The 
   second and third arguments give the unique name to delete.  In
   this example the unique name is the concatenation of the object instance and the variable name.
   retore_stl will delete the memory manager arrays. 
 */
int STLCompanion::restart(STLCheckpoint * stlc, std::string object_name ) {

    restore_stl(stlc->my_double_map , object_name , std::string("my_double_map")) ;
    restore_stl(stlc->my_string_key_map , object_name , std::string("my_string_key_map")) ;
    restore_stl(stlc->my_string_data_map , object_name , std::string("my_string_data_map")) ;
    restore_stl(stlc->my_string_map , object_name , std::string("my_string_map")) ;

    restore_stl(stlc->my_int_multimap , object_name , std::string("my_int_multimap")) ;
    restore_stl(stlc->my_string_key_multimap , object_name , std::string("my_string_key_multimap")) ;
    restore_stl(stlc->my_string_data_multimap , object_name , std::string("my_string_data_multimap")) ;
    restore_stl(stlc->my_string_multimap , object_name , std::string("my_string_multimap")) ;

    restore_stl(stlc->my_double_vector , object_name , std::string("my_double_vector")) ;
    restore_stl(stlc->my_string_vector , object_name , std::string("my_string_vector")) ;

    restore_stl(stlc->my_short_list , object_name , std::string("my_short_list")) ;
    restore_stl(stlc->my_string_list , object_name , std::string("my_string_list")) ;

    restore_stl(stlc->my_float_deque , object_name , std::string("my_float_deque")) ;
    restore_stl(stlc->my_string_deque , object_name , std::string("my_string_deque")) ;

    restore_stl(stlc->my_int_set , object_name , std::string("my_int_set")) ;
    restore_stl(stlc->my_string_set , object_name , std::string("my_string_set")) ;

    restore_stl(stlc->my_long_multiset , object_name , std::string("my_long_multiset")) ;
    restore_stl(stlc->my_string_multiset , object_name , std::string("my_string_multiset")) ;

    restore_stl(stlc->my_uint_stack , object_name , std::string("my_uint_stack")) ;
    restore_stl(stlc->my_string_stack , object_name , std::string("my_string_stack")) ;

    restore_stl(stlc->my_int_queue , object_name , std::string("my_int_queue")) ;
    restore_stl(stlc->my_string_queue , object_name , std::string("my_string_queue")) ;

    restore_stl(stlc->my_int_priority_queue , object_name , std::string("my_int_priority_queue")) ;
    restore_stl(stlc->my_string_priority_queue , object_name , std::string("my_string_priority_queue")) ;

    restore_stl(stlc->my_int_pair , object_name , std::string("my_int_pair")) ;
    restore_stl(stlc->my_string_first_pair , object_name , std::string("my_string_first_pair")) ;
    restore_stl(stlc->my_string_second_pair , object_name , std::string("my_string_second_pair")) ;
    restore_stl(stlc->my_string_pair , object_name , std::string("my_stringpair")) ;

    return 0 ;
}
