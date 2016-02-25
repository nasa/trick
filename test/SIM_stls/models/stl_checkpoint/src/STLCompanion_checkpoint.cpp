
#include <string>

#include "STLCompanion.hh"
#include "sim_services/CheckPointRestart/include/checkpoint_stl.hh"

/* All STLs are checkpointed through the same templated function checkpoint_stl.  The 
   second and third arguments to the call give each set of STL data a unique name.  In
   this example the unique name is the concatenation of the object instance and the variable name
 */
int STLCompanion::checkpoint(STLCheckpoint * stlc, std::string object_name ) {

    checkpoint_stl(stlc->my_double_map , object_name , std::string("my_double_map")) ;
    checkpoint_stl(stlc->my_string_key_map , object_name , std::string("my_string_key_map")) ;
    checkpoint_stl(stlc->my_string_data_map , object_name , std::string("my_string_data_map")) ;
    checkpoint_stl(stlc->my_string_map , object_name , std::string("my_string_map")) ;

    checkpoint_stl(stlc->my_int_multimap , object_name , std::string("my_int_multimap")) ;
    checkpoint_stl(stlc->my_string_key_multimap , object_name , std::string("my_string_key_multimap")) ;
    checkpoint_stl(stlc->my_string_data_multimap , object_name , std::string("my_string_data_multimap")) ;
    checkpoint_stl(stlc->my_string_multimap , object_name , std::string("my_string_multimap")) ;

    checkpoint_stl(stlc->my_double_vector , object_name , std::string("my_double_vector")) ;
    checkpoint_stl(stlc->my_string_vector , object_name , std::string("my_string_vector")) ;

    checkpoint_stl(stlc->my_short_list , object_name , std::string("my_short_list")) ;
    checkpoint_stl(stlc->my_string_list , object_name , std::string("my_string_list")) ;

    checkpoint_stl(stlc->my_float_deque , object_name , std::string("my_float_deque")) ;
    checkpoint_stl(stlc->my_string_deque , object_name , std::string("my_string_deque")) ;

    checkpoint_stl(stlc->my_int_set , object_name , std::string("my_int_set")) ;
    checkpoint_stl(stlc->my_string_set , object_name , std::string("my_string_set")) ;

    checkpoint_stl(stlc->my_long_multiset , object_name , std::string("my_long_multiset")) ;
    checkpoint_stl(stlc->my_string_multiset , object_name , std::string("my_string_multiset")) ;

    checkpoint_stl(stlc->my_uint_stack , object_name , std::string("my_uint_stack")) ;
    checkpoint_stl(stlc->my_string_stack , object_name , std::string("my_string_stack")) ;

    checkpoint_stl(stlc->my_int_queue , object_name , std::string("my_int_queue")) ;
    checkpoint_stl(stlc->my_string_queue , object_name , std::string("my_string_queue")) ;

    checkpoint_stl(stlc->my_int_priority_queue , object_name , std::string("my_int_priority_queue")) ;
    checkpoint_stl(stlc->my_string_priority_queue , object_name , std::string("my_string_priority_queue")) ;

    checkpoint_stl(stlc->my_int_pair , object_name , std::string("my_int_pair")) ;
    checkpoint_stl(stlc->my_string_first_pair , object_name , std::string("my_string_first_pair")) ;
    checkpoint_stl(stlc->my_string_second_pair , object_name , std::string("my_string_second_pair")) ;
    checkpoint_stl(stlc->my_string_pair , object_name , std::string("my_stringpair")) ;

    return 0 ;
}
