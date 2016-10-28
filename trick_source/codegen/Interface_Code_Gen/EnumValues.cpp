
#include <iostream>

#include "EnumValues.hh"

EnumValues::EnumValues() : has_definition(true) {}

void EnumValues::addEnum( std::string in_name , long long in_val ) {
    std::pair< std::string , long long > new_enum(in_name, in_val) ;
    enum_values.push_back(new_enum) ;
}

void EnumValues::setHasDefinition( bool in ) {
    has_definition = in ;
}

bool EnumValues::getHasDefinition() {
    return has_definition ;
}
