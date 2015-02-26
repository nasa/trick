/*
 * $Id: AttributesMap.cpp 2073 2011-12-09 21:05:52Z lin $
 */

#include "sim_services/Sie/include/Sie.hh"
#include "sim_services/Sie/include/sie_c_intf.h"

extern Trick::Sie * the_sie ;

extern "C" void sie_print_xml() {
    if ( the_sie != NULL ) {
        the_sie->sie_print_xml() ;
    }
}

extern "C" void sie_class_attr_map_print_xml() {
    if ( the_sie != NULL ) {
        the_sie->class_attr_map_print_xml() ;
    }
}

extern "C" void sie_enum_attr_map_print_xml() {
    if ( the_sie != NULL ) {
        the_sie->enum_attr_map_print_xml() ;
    }
}

extern "C" void sie_top_level_objects_print_xml() {
    if ( the_sie != NULL ) {
        the_sie->top_level_objects_print_xml() ;
    }
}
