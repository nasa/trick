
#include <iostream>
#include <sstream>

#include "ClassValues.hh"
#include "FieldDescription.hh"

ClassValues::ClassValues(bool in_inherit, bool in_virtual_inherit) :
  inherited(in_inherit) ,
  virtual_inherited(in_virtual_inherit) ,
  has_init_attr_friend(false) ,
  is_pod(false) ,
  is_abstract(false) ,
  has_default_constructor(false) ,
  has_public_destructor(false) {}

ClassValues::~ClassValues() {
    std::vector<FieldDescription *>::iterator fdit ;
    for ( fdit = field_descripts.begin() ; fdit != field_descripts.end() ; fdit++ ) {
        delete (*fdit) ;
    }

    std::vector<ClassValues *>::iterator cvit ;
    for ( cvit = inherited_classes.begin() ; cvit != inherited_classes.end() ; cvit++ ) {
        delete (*cvit) ;
    }
}

void ClassValues::addFieldDescription(FieldDescription * in_fdes) {
    field_descripts.push_back(in_fdes) ;
    field_name_to_info_map[in_fdes->getName()] = in_fdes ;
}

void ClassValues::addInheritedFieldDescriptions(std::vector<FieldDescription *> in_fdes) {
    // Make a copy of all of the FieldDescription variables.
    field_descripts.insert(field_descripts.end(), in_fdes.begin() , in_fdes.end()) ;

    std::vector<FieldDescription *>::iterator fdit ;
    // Loop through the incoming inherited variable names
    for ( fdit = in_fdes.begin() ; fdit != in_fdes.end() ; fdit++ ) {
        std::string in_name = (*fdit)->getName() ;

        // search existing names for incoming inherited variable name.
        std::map< std::string , FieldDescription * >::iterator mit = field_name_to_info_map.find(in_name) ;
        // if the variable name already exists we have overloaded variable names.
        if ( mit != field_name_to_info_map.end() ) {
            // the incoming variable is known to be inherited.  Qualify it with its container class name.
            (*fdit)->setName( (*fdit)->getContainerClass() + "::" + (*fdit)->getName() ) ;
            field_name_to_info_map[(*fdit)->getName()] = *fdit ;

            // If the matched variable is also inherited, qualify it with its container class name.
            if ( (*mit).second->isInherited() ) {
                (*mit).second->setName( (*mit).second->getContainerClass() + "::" + (*mit).second->getName() ) ;
                field_name_to_info_map[(*mit).second->getName()] = (*mit).second ;
                field_name_to_info_map.erase(mit) ;
                // Mark this name is one that we always have to qualify.
                field_names_to_qualify.insert(in_name) ;
            }
        } else {
            // incoming name did not match an existing variable name.
            // Test to see if there variables of this name need to be qualified.
            if ( field_names_to_qualify.find(in_name) == field_names_to_qualify.end() ) {
                // The name is not overloaded (yet), add the unqualified inherited name straight into map.
                field_name_to_info_map[in_name] = *fdit ;
                // Upgrade the inherited variable's container class to the current class.
                //(*fdit)->setContainerClass( name ) ;
            } else {
                // The name is overloaded by other inherited variables... qualify the name.
                (*fdit)->setName( (*fdit)->getContainerClass() + "::" + (*fdit)->getName() ) ;
                field_name_to_info_map[(*fdit)->getName()] = *fdit ;
            }
        }
    }

}

void ClassValues::saveInheritAncestry( ClassValues * in_cv  ) {

    std::map< std::string , unsigned int >::iterator mit ;
    for ( mit = in_cv->all_inherited_class_names_map.begin() ; mit != in_cv->all_inherited_class_names_map.end() ; mit++ ) {
        all_inherited_class_names_map[(*mit).first] +=  (*mit).second ;
    }

    all_inherited_class_names_map[in_cv->getName()]++ ;
}

void ClassValues::setContainerClassForFields() {
    std::vector<FieldDescription *>::iterator fdit ;
    // Loop through all fields
    for ( fdit = field_descripts.begin() ; fdit != field_descripts.end() ; fdit++ ) {
        // Prepend the field container class with the current class name.
        (*fdit)->setContainerClass( name + "::" + (*fdit)->getContainerClass() ) ;
    }
}

void ClassValues::clearAmbiguousVariables() {

    std::map< std::string , unsigned int >::iterator mit ;
    std::vector<FieldDescription *>::iterator fdit ;

    for ( mit = all_inherited_class_names_map.begin() ; mit != all_inherited_class_names_map.end() ; mit++ ) {
        // If a class in inherited more than once we have a diamond.  We'll need to modify variables that were
        // inherited from that class.
        if ( (*mit).second > 1 ) {
            std::string str = (*mit).first + "::" ;
            // Loop through all fields testing for the diamond inherited class in the name.
            for ( fdit = field_descripts.begin() ; fdit != field_descripts.end() ; fdit++ ) {
                std::string fdit_name = (*fdit)->getName() ;
                size_t f = fdit_name.find(str) ;
                // If the variable contains the diamond class string, remove the diamond class qualification.
                if ( f != std::string::npos ) {
                    fdit_name.replace(f , str.length() , "") ;
                    (*fdit)->setName(fdit_name) ;
                }
            }
        }
    }

    // Removing diamond class qualifications possibly leaves fields with duplicate names.
    // We want to remove the duplicated names.
    std::set< std::string > field_names ;
    fdit = field_descripts.begin() ;
    while ( fdit != field_descripts.end() ) {
        // test if the current field is in the list of field names processed.
        if ( field_names.find( (*fdit)->getName() ) == field_names.end() ) {
            // This name is new.
            // Add the field to the test set
            field_names.insert( (*fdit)->getName() ) ;
            fdit++ ;
        } else {
            // This name was already present
            // Free the field's memory.
            delete (*fdit) ;
            // Erase the field from the list. Move iterator to next field.
            fdit = field_descripts.erase(fdit) ;

        }
    }

}

std::vector<FieldDescription *> ClassValues::getFieldDescription() {
    return field_descripts ;
}

void ClassValues::clearFieldDescription() {
    field_descripts.clear() ;
}

void ClassValues::addInheritedClass(ClassValues * in_cv) {
    inherited_classes.push_back(in_cv) ;
}

void ClassValues::clearInheritedClass() {
    inherited_classes.clear() ;
}

void ClassValues::setInherited(bool in_inh) {
    inherited = in_inh ;
}

bool ClassValues::isInherited() {
    return inherited ;
}

void ClassValues::setVirtualInherited(bool in_inh) {
    virtual_inherited = in_inh ;
}

bool ClassValues::isVirtualInherited() {
    return virtual_inherited ;
}

void ClassValues::setHasInitAttrFriend(bool in_val) {
    has_init_attr_friend = in_val ;
}

bool ClassValues::getHasInitAttrFriend() {
    return has_init_attr_friend ;
}

void ClassValues::setPOD(bool in_val) {
    is_pod = in_val ;
}

bool ClassValues::isPOD() {
    return is_pod ;
}

void ClassValues::setAbstract(bool in_val) {
    is_abstract = in_val ;
}

bool ClassValues::isAbstract() {
    return is_abstract ;
}

void ClassValues::setHasDefaultConstructor(bool in_val) {
    has_default_constructor = in_val ;
}

bool ClassValues::getHasDefaultConstructor() {
    return has_default_constructor ;
}

void ClassValues::setHasPublicDestructor(bool in_val) {
    has_public_destructor = in_val ;
}

bool ClassValues::getHasPublicDestructor() {
    return has_public_destructor ;
}

void ClassValues::setMangledTypeName( std::string in_val ) {
    mangled_type_name = in_val ;
}

std::string ClassValues::getMangledTypeName() {
    if ( mangled_type_name.empty() ) {
        return name ;
    }
    return mangled_type_name ;
}

std::string ClassValues::getFullyQualifiedMangledTypeName() {
    std::ostringstream oss ;
    NamespaceIterator ni ;

    for ( ni = namespace_begin() ; ni != namespace_end() ; ni++ ) {
        oss << (*ni) << "::" ;
    }
    for ( ni = container_class_begin() ; ni != container_class_end() ; ni++ ) {
        oss << (*ni) << "::" ;
    }
    oss << getMangledTypeName() ;
    return oss.str() ;
}

void ClassValues::print_namespaces(std::ostream & os, const char * delimiter) {
    unsigned int ii ;
    for ( ii = 0 ; ii < namespaces.size() ; ii++ ) {
        os << namespaces[ii] << delimiter ;
    }
}

std::ostream & operator << (std::ostream & os , ClassValues & cv ) {
    os << "  name = " << cv.name << std::endl ;
    os << "  mangled_name = " << cv.mangled_type_name << std::endl ;
    os << "  file_name = " << cv.file_name << std::endl ;
    os << "  namespaces =" ;
    ConstructValues::NamespaceIterator it ;
    for ( it = cv.namespace_begin() ; it != cv.namespace_end() ; it++ ) {
        os << " " << *it ;
    }
    os << std::endl ;
    os << "  parent classes =" ;
    for ( it = cv.container_class_begin() ; it != cv.container_class_end() ; it++ ) {
        os << " " << *it ;
    }
    os << std::endl ;
    os << "  has_init_attr_friend = " << cv.has_init_attr_friend << std::endl ;
    os << "  is_pod = " << cv.is_pod << std::endl ;
    os << "  is_abstract = " << cv.is_abstract << std::endl ;
    os << "  has_default_constructor = " << cv.has_default_constructor << std::endl ;
    os << "  has_public_destructor = " << cv.has_public_destructor << std::endl ;

    ClassValues::FieldIterator fit ;
    for ( fit = cv.field_begin() ; fit != cv.field_end() ; fit++ ) {
        os << **fit << std::endl ;
    }

    return os ;
}
