
#include <iostream>
#include <algorithm>
#include <fstream>
#include <string.h>
#include <cstring>

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
            sie_print_json();

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
            sie_out << "\n   name=\"" << vit->first << "\"" ;
            sie_out << "\n   type=\"" ;
            std::string type = trickTypeCharString(alloc_info->type, alloc_info->user_type_name );
            std::replace(type.begin(), type.end(), ':', '_');
            sie_out <<  type << "\"\n" ;
            sie_out << "   alloc_memory_init=\"" << alloc_info->alloced_in_memory_init << "\"";
            sie_out << ">\n" ;
            if ( alloc_info->num_index > 0 ) {
                for (jj = 0; jj < alloc_info->num_index; jj++) {
                    sie_out << "        <dimension>" << alloc_info->index[jj] << "</dimension>\n" ;
                }
            }
            sie_out << "  </top_level_object>\n\n" ;
        }
    }
}

void Trick::Sie::runtime_objects_print(std::fstream & sie_out) {
    Trick::VARIABLE_MAP_ITER vit ;
    int jj ;

    for ( vit = trick_MM->variable_map_begin() ; vit != trick_MM->variable_map_end() ; vit++ ) {
        ALLOC_INFO * alloc_info = (*vit).second ;

        if ( alloc_info != NULL && alloc_info->alloced_in_memory_init == 0) {
            sie_out << "  <top_level_object" ;
            sie_out << "\n   name=\"" << vit->first << "\"" ;
            sie_out << "\n   type=\"" ;
            std::string type = trickTypeCharString(alloc_info->type, alloc_info->user_type_name );
            std::replace(type.begin(), type.end(), ':', '_');
            sie_out <<  type << "\"\n" ;
            sie_out << "   alloc_memory_init=\"" << alloc_info->alloced_in_memory_init << "\"";
            sie_out << ">\n" ;
            if ( alloc_info->num_index > 0 ) {
                for (jj = 0; jj < alloc_info->num_index; jj++) {
                    sie_out << "        <dimension>" << alloc_info->index[jj] << "</dimension>\n" ;
                }
            }
            sie_out << "  </top_level_object>\n\n" ;
        }
    }
}

void Trick::Sie::top_level_objects_json(std::ofstream & sie_out) {
    Trick::VARIABLE_MAP_ITER vit ;
    int jj ;
    sie_out << "  \"top_level_objects\": [\n";
    for ( vit = trick_MM->variable_map_begin() ; vit != trick_MM->variable_map_end() ; vit++ ) {
        ALLOC_INFO * alloc_info = (*vit).second ;

        if ( alloc_info != NULL ) {
            sie_out << "    {\n" ;
            sie_out << "      \"name\": \"" << vit->first << "\",\n" ;
            sie_out << "      \"type\": \"" ;
            std::string type = trickTypeCharString(alloc_info->type, alloc_info->user_type_name );
            std::replace(type.begin(), type.end(), ':', '_') ;
            sie_out <<  type << "\",\n" ;
            sie_out << "      \"alloc_memory_init\": \"" << alloc_info->alloced_in_memory_init << "\"";
            if ( alloc_info->num_index > 0 ) {
                sie_out << ",\n        \"dimensions\": [" ;
                for (jj = 0; jj < alloc_info->num_index - 1; jj++) {
                    sie_out << " \"" << alloc_info->index[jj] << "\"," ;
                }
                sie_out << " \"" << alloc_info->index[alloc_info->num_index - 1] << "\" " ;
                sie_out << "]\n" ;
                } else {
                    sie_out << '\n' ;
                }
            sie_out << "    }" ;
            if(std::next(vit, 1) != trick_MM->variable_map_end()) {
                sie_out << ',' ;
            }
            sie_out << '\n';
        }
    }
    sie_out << "  ]\n";
}


void Trick::Sie::sie_print_xml() {
    std::ofstream sie_out ;
    std::string file_name = std::string(command_line_args_get_default_dir()) + "/" + "S_sie.resource" ;
    sie_out.open(file_name.c_str()) ;
    sie_out << "<?xml version=\"1.0\"?>\n\n" ;
    sie_out << "<sie>\n\n" ;
    class_attr_map->print_xml(sie_out) ;
    enum_attr_map->print_xml(sie_out) ;
    top_level_objects_print(sie_out) ;
    sie_out << "</sie>\n" ;
    sie_out.close() ;
}

void Trick::Sie::sie_append_runtime_objs() {
    std::fstream sie_out ;
    std::string file_name = std::string(command_line_args_get_default_dir()) + "/" + "S_sie.resource" ;
    sie_out.open(file_name.c_str(), std::fstream::in | std::fstream::out) ;
    sie_out.seekg(-1, sie_out.end);
    const char * comment = "<!--\nRuntime Allocations\nDo not edit this comment or file content past this point\n-->\n";
    const int commentLength = 86;
    char buff[commentLength + 1] = {0};
    char last = '\0';
    int pos;
    while(memcmp(comment, buff, commentLength) != 0) {
        while(last != '!' || sie_out.peek() != '<') {
            if(sie_out.bad() || sie_out.fail() || sie_out.eof()) {
                std::cerr << "Warning: Cannot add runtime/dynamic allocations to S_sie.resource. S_sie.resource is corrupted, outdated, or missing. Please be sure that SIM_*/S_sie.resource is preserved after build time if needed at runtime for trick-tv or other variable server clients. Please also rerun trick-CP." << std::endl;
                return;
            }
            last = sie_out.peek();
            sie_out.seekg(-1, std::ios::cur);
            pos = sie_out.tellg();
        }
        sie_out.get(buff, commentLength + 1, '\0');
        sie_out.seekg(pos - 1);
    }
    sie_out.seekg(pos - 1);
    sie_out.seekp((int)sie_out.tellg() + commentLength + 1);
    runtime_objects_print(sie_out);
    sie_out << "</sie>\n";
    sie_out.close();
}

void Trick::Sie::sie_print_json() {
    std::ofstream sie_out ;
    std::string file_name = std::string(command_line_args_get_default_dir()) + "/" + "S_sie.json" ;
    sie_out.open(file_name.c_str()) ;
    sie_out << "{\n" ;
    class_attr_map->print_json(sie_out) ;
    enum_attr_map->print_json(sie_out) ;
    top_level_objects_json(sie_out) ;
    sie_out << "}\n" ;
    sie_out.close() ;
}


void Trick::Sie::class_attr_map_print_xml() {
    std::ofstream sie_out ;
    std::string file_name = std::string(command_line_args_get_default_dir()) + "/" + "S_sie_class.xml" ;
    sie_out.open(file_name.c_str()) ;
    sie_out << "<?xml version=\"1.0\"?>\n\n" ;
    sie_out << "<sie>\n" ;
    class_attr_map->print_xml(sie_out) ;
    sie_out << "</sie>\n" ;
    sie_out.close() ;
}

void Trick::Sie::enum_attr_map_print_xml() {
    std::ofstream sie_out ;
    std::string file_name = std::string(command_line_args_get_default_dir()) + "/" + "S_sie_enum.xml" ;
    sie_out.open(file_name.c_str()) ;
    sie_out << "<?xml version=\"1.0\"?>\n\n" ;
    sie_out << "<sie>\n" ;
    enum_attr_map->print_xml(sie_out) ;
    sie_out << "</sie>\n" ;
    sie_out.close() ;
}

void Trick::Sie::top_level_objects_print_xml() {
    std::ofstream sie_out ;
    std::string file_name = std::string(command_line_args_get_default_dir()) + "/" + "S_sie_top_level_objects.xml" ;
    sie_out.open(file_name.c_str()) ;
    sie_out << "<?xml version=\"1.0\"?>\n\n" ;
    sie_out << "<sie>\n" ;
    top_level_objects_print(sie_out) ;
    sie_out << "</sie>\n" ;
    sie_out.close() ;
}

