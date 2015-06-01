#include <iostream>
#include <fstream>
#include <algorithm>
#include "trick/EnumAttributesMap.hh"
#include "trick/attributes.h"
#include "trick/command_line_protos.h"

// Instantiate static variables for template types for ATTRIBUTES * and ENUM_ATTR *
namespace Trick {
EnumAttributesMap * EnumAttributesMap::pInstance = NULL ;
}

void Trick::EnumAttributesMap::print_xml(std::ofstream & sie_out ) {
    std::map<std::string, ENUM_ATTR *>::iterator it ;
    ENUM_ATTR * enum_attr ;

    for ( it = name_to_attr_map.begin() ; it != name_to_attr_map.end() ; it++ ) {
        enum_attr = (*it).second ;

        if ( enum_attr != NULL ) {
            std::string name = it->first;
            std::replace(name.begin(), name.end(), ':', '_');
            sie_out << "  <enumeration name=\"" << name << "\">" << std::endl ;
            while ( enum_attr->label[0] != '\0' ) {
                sie_out << "      <pair label=\"" << enum_attr->label << "\"" ;
                sie_out << " value=\"" << enum_attr->value << "\"/>" << std::endl ;
                enum_attr++ ;
            }
            sie_out << "  </enumeration>" << std::endl << std::endl ;
        }
    }
}

