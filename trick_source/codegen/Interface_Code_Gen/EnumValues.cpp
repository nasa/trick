
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

std::ostream & operator << (std::ostream & os , EnumValues & ev ) {
    os << "    name = " << ev.name << std::endl ;
    os << "    file_name = " << ev.file_name << std::endl ;
    os << "    namespaces =" ;
    ConstructValues::NamespaceIterator it ;
    for ( it = ev.namespace_begin() ; it != ev.namespace_end() ; it++ ) {
        os << " " << *it ;
    }
    os << std::endl ;
    os << "    parent classes =" ;
    for ( it = ev.container_class_begin() ; it != ev.container_class_end() ; it++ ) {
        os << " " << *it ;
    }
    os << std::endl ;

    EnumValues::NameValueIterator eit ;
    for ( eit = ev.begin() ; eit != ev.end() ; eit++ ) {
        os << "      " << (*eit).first << " " << (*eit).second << std::endl ;
    }

    return os ;
}
