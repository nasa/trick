
#include <iostream>
#include <algorithm>
#include <fstream>
#include <string.h>

#include "trick/Sie.hh"
#include "trick/SimObject.hh"
#include "trick/attributes.h"
#include "trick/reference.h"
#include "trick/memorymanager_c_intf.h"
#include "trick/exec_proto.hh"
#include "trick/command_line_protos.h"
#include "trick/MemoryManager.hh"
#include "trick/parameter_types.h"

Trick::Sie * the_sie = NULL ;

Trick::Sie::Sie() {
    // Call the attribute_map function which will instantiate the singleton maps
    class_attr_map = Trick::AttributesMap::attributes_map() ;
    enum_attr_map = Trick::EnumAttributesMap::attributes_map() ;

    the_sie = this ;
}

int Trick::Sie::process_sim_args() {

    int argc ;
    char ** argv ;

    argc = command_line_args_get_argc() ;
    argv = command_line_args_get_argv() ;

    if (argc >= 2) {
        if (!strcmp(argv[1], "sie")) {
            /* If main is being invoked by the configuration processor (cp) to generate the sie resource file... */
            /* Generate the sie resource file */
            sie_print_xml();

            // Silently exit the sim without printing the termination message
            exit(0) ;
        }
    }

    return(0) ;

}

void Trick::Sie::top_level_objects_print(std::ofstream & sie_out) {
    Trick::VARIABLE_MAP_ITER vit ;
    int jj ;

    for ( vit = trick_MM->variable_map_begin() ; vit != trick_MM->variable_map_end() ; vit++ ) {
        ALLOC_INFO * alloc_info = (*vit).second ;

        if ( alloc_info != NULL ) {
            sie_out << "  <top_level_object" ;
            sie_out << std::endl << "   name=\"" << vit->first << "\"" ;
            sie_out << std::endl << "   type=\"" ;
            std::string type = trickTypeCharString(alloc_info->type, alloc_info->user_type_name );
            std::replace(type.begin(), type.end(), ':', '_');
            sie_out <<  type << "\"" << std::endl ;
            sie_out << "   alloc_memory_init=\"" << alloc_info->alloced_in_memory_init << "\"";
            sie_out << ">" << std::endl ;
            if ( alloc_info->num_index > 0 ) {
                for (jj = 0; jj < alloc_info->num_index; jj++) {
                    sie_out << "        <dimension>" << alloc_info->index[jj] << "</dimension>" << std::endl ;
                }
            }
            sie_out << "  </top_level_object>" << std::endl << std::endl ;
        }
    }
}

void Trick::Sie::sie_print_xml() {
    std::ofstream sie_out ;
    std::string file_name = std::string(command_line_args_get_default_dir()) + "/" + "S_sie.resource" ;
    sie_out.open(file_name.c_str()) ;
    sie_out << "<?xml version=\"1.0\"?>" << std::endl << std::endl ;
    sie_out << "<sie>" << std::endl << std::endl ;
    class_attr_map->print_xml(sie_out) ;
    enum_attr_map->print_xml(sie_out) ;
    top_level_objects_print(sie_out) ;
    sie_out << "</sie>" << std::endl ;
    sie_out.close() ;
}

void Trick::Sie::class_attr_map_print_xml() {
    std::ofstream sie_out ;
    std::string file_name = std::string(command_line_args_get_default_dir()) + "/" + "S_sie_class.xml" ;
    sie_out.open(file_name.c_str()) ;
    sie_out << "<?xml version=\"1.0\"?>" << std::endl << std::endl ;
    sie_out << "<sie>" << std::endl ;
    class_attr_map->print_xml(sie_out) ;
    sie_out << "</sie>" << std::endl ;
    sie_out.close() ;
}

void Trick::Sie::enum_attr_map_print_xml() {
    std::ofstream sie_out ;
    std::string file_name = std::string(command_line_args_get_default_dir()) + "/" + "S_sie_enum.xml" ;
    sie_out.open(file_name.c_str()) ;
    sie_out << "<?xml version=\"1.0\"?>" << std::endl << std::endl ;
    sie_out << "<sie>" << std::endl ;
    enum_attr_map->print_xml(sie_out) ;
    sie_out << "</sie>" << std::endl ;
    sie_out.close() ;
}

void Trick::Sie::top_level_objects_print_xml() {
    std::ofstream sie_out ;
    std::string file_name = std::string(command_line_args_get_default_dir()) + "/" + "S_sie_top_level_objects.xml" ;
    sie_out.open(file_name.c_str()) ;
    sie_out << "<?xml version=\"1.0\"?>" << std::endl << std::endl ;
    sie_out << "<sie>" << std::endl ;
    top_level_objects_print(sie_out) ;
    sie_out << "</sie>" << std::endl ;
    sie_out.close() ;
}

