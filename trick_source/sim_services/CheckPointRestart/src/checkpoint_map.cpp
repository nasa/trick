
#include <string>
#include <map>

#include "sim_services/CheckPointRestart/include/checkpoint_map.hh"

int checkpoint_stl(std::map<std::string , std::string> & in_map , std::string object_name , std::string var_name ) {
    return checkpoint_map_stl_strings( in_map , object_name , var_name ) ;
}

int checkpoint_stl(std::multimap<std::string , std::string> & in_map , std::string object_name , std::string var_name ) {
    return checkpoint_map_stl_strings( in_map , object_name , var_name ) ;
}

int restore_stl(std::map<std::string , std::string> & in_map , std::string object_name , std::string var_name ) {
    return restore_map_stl_strings( in_map , object_name , var_name ) ;
}

int restore_stl(std::multimap<std::string , std::string> & in_map , std::string object_name , std::string var_name ) {
    return restore_map_stl_strings( in_map , object_name , var_name ) ;
}

