#include <iostream>
#include <fstream>
#include <algorithm>
#include "trick/AttributesMap.hh"
#include "trick/attributes.h"
#include "trick/command_line_protos.h"

// Instantiate static variables for template types for ATTRIBUTES * and ENUM_ATTR *
namespace Trick {
AttributesMap * AttributesMap::pInstance = NULL ;
}

std::string & Trick::AttributesMap::replace_special_chars( std::string & str) {

    // escape &
    size_t index = 0;
    while (index != std::string::npos) {
        index = str.find("&" , index) ;
        if ( index != std::string::npos ) {
            str.replace(index, 1, "&amp;") ;
            index += 5;
        }
    }

    // escape "
    index = 0;
    while (index != std::string::npos) {
        index = str.find("\"" , index) ;
        if ( index != std::string::npos ) {
            str.replace(index, 1, "&quot;") ;
        }
    }

    // escape <
    index = 0;
    while (index != std::string::npos) {
        index = str.find("<" , index) ;
        if ( index != std::string::npos ) {
            str.replace(index, 1, "&lt;") ;
        }
    }

    return str;
}

std::string & Trick::AttributesMap::type_remove_dims( std::string & type ) {
    size_t index = type.find_first_of("*[");
    if (index != std::string::npos) {
        type.erase(index);
    }
    index = type.find_last_not_of(" ");
    if (index != std::string::npos) {
        type.erase(index + 1);
    }

    return replace_special_chars(type) ;
}

void Trick::AttributesMap::print_xml(std::ofstream & sie_out ) {
    std::map<std::string, ATTRIBUTES *>::iterator it ;
    int jj ;

    for ( it = name_to_attr_map.begin() ; it != name_to_attr_map.end() ; it++ ) {
        ATTRIBUTES * attr = (*it).second ;
        std::string class_name = (*it).first;
        std::replace(class_name.begin(), class_name.end(), ':', '_');
        sie_out << "  <class name=\"" <<  class_name << "\">" << std::endl ;
        while ( attr->name[0] != '\0' and (attr->type_name != NULL)) {
            sie_out << "    <member" ;
            sie_out << std::endl << "      name=\"" << attr->name << "\"" ;
            std::string type_name = attr->type_name;
            std::replace(type_name.begin(), type_name.end(), ':', '_');
            sie_out << std::endl << "      type=\"" << type_remove_dims(type_name) << "\"" ;
            sie_out << std::endl << "      io_attributes=\"" << attr->io << "\"" ;
            sie_out << std::endl << "      units=\"" ;
            // If the mods bit is set for using -- as the units
            if ( attr->mods & TRICK_MODS_UNITSDASHDASH ) {
                sie_out << "--" ;
            } else {
                sie_out << attr->units ;
            }
            sie_out << "\"" ;

            std::string description = attr->des;
            if ( ! description.empty() ) {
                sie_out << std::endl << "      description=\"" << replace_special_chars(description) << "\"" ;
            }
            sie_out << ">" << std::endl ;
            if ( attr->num_index > 0 ) {
                for (jj = 0; jj < attr->num_index; jj++) {
                    sie_out << "      <dimension>" << attr->index[jj].size << "</dimension>" << std::endl ;
                }
            }
            sie_out << "    </member>" << std::endl ;
            attr++ ;
        }
        sie_out << "  </class>" << std::endl << std::endl ;
    }
}

