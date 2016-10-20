
#include <sys/stat.h>
#include <libgen.h>
#include <string.h>
#include <iomanip>
#include <sstream>

#include "llvm/Support/CommandLine.h"

#include "PrintFileContentsBase.hh"
#include "FieldDescription.hh"
#include "ClassValues.hh"
#include "EnumValues.hh"

extern llvm::cl::opt< bool > no_offset_of ;

PrintFileContentsBase::PrintFileContentsBase() {}

// provide empty default implementation of these routines.
void PrintFileContentsBase::printClassMapHeader(std::ostream & ostream, std::string function_name ) {}
void PrintFileContentsBase::printClassMap(std::ostream & ostream, ClassValues * cv) {}
void PrintFileContentsBase::printClassMapFooter(std::ostream & ostream) {}

void PrintFileContentsBase::printEnumMapHeader(std::ostream & ostream, std::string function_name ) {}
void PrintFileContentsBase::printEnumMap(std::ostream & ostream, EnumValues * ev) {}
void PrintFileContentsBase::printEnumMapFooter(std::ostream & ostream) {}

void PrintFileContentsBase::print_units_map(std::ostream & ostream, ClassValues * cv ) {
    ClassValues::FieldIterator fit ;
    unsigned int ii ;

    ostream << "struct UnitsMap" ;
    printNamespaces( ostream, cv , "__" ) ;
    printContainerClasses( ostream, cv , "__" ) ;
    ostream << cv->getMangledTypeName() ;
    ostream << " {\n" ;

    std::ostringstream text;

    for ( fit = cv->field_begin() ; fit != cv->field_end() ; fit++ ) {
        if ( determinePrintAttr(cv , *fit) and (*fit)->getUnits().compare("1")) {
            FieldDescription * fdes = *fit ;
            text << "        units_map_ptr->add_param(\"" ;
            printContainerClasses( text, cv , "__" ) ;
            text << cv->getName() << "_" << fdes->getName() << "\", \"" << fdes->getUnits() << "\") ;\n" ;
        }
    }

    if (text.str().size()) {
        ostream << "    UnitsMap" ;
        printNamespaces( ostream, cv , "__" ) ;
        printContainerClasses( ostream, cv , "__" ) ;
        ostream << cv->getMangledTypeName() ;
        ostream << "() {\n" ;
        ostream << "        Trick::UnitsMap * units_map_ptr = Trick::UnitsMap::units_map() ;\n" ;
        ostream << text.str() ;
        ostream << "    }\n" ;
    }
    ostream << "} um" ;
    printNamespaces( ostream, cv , "__" ) ;
    printContainerClasses( ostream, cv , "__" ) ;
    ostream << cv->getMangledTypeName() ;
    ostream << " ;\n" ;
}

/* Utility routines for printing */
void PrintFileContentsBase::print_open_extern_c(std::ostream & ostream) {
    ostream << "extern \"C\" {\n\n" ;
}

void PrintFileContentsBase::print_close_extern_c(std::ostream & ostream) {
    ostream << "\n} //extern \"C\"\n\n" ;
}

/* internal function determines if a particular field is printable based
   on access to the field and the presense of init_attr friends.
*/
bool PrintFileContentsBase::determinePrintAttr( ClassValues * c , FieldDescription * fdes ) {
    if ( fdes->getTypeName().compare("void") and fdes->getIO() != 0 and fdes->getEnumString().compare("TRICK_VOID")) {
        if ( no_offset_of ) {
            if ( fdes->isStatic() ) {
                if ( fdes->isInherited() ) {
                    return ((c->getHasInitAttrFriend() && fdes->getAccess() == clang::AS_protected)
                         || (fdes->getAccess() == clang::AS_public)) ;
                } else {
                    return (c->getHasInitAttrFriend()
                        || (fdes->getAccess() == clang::AS_public)) ;
                }
            } else {
                return true ;
            }
        } else {
            if ( fdes->isInherited() ) {
                return ((c->getHasInitAttrFriend() && fdes->getAccess() == clang::AS_protected)
                     || (fdes->getAccess() == clang::AS_public)) ;
            } else {
                return (c->getHasInitAttrFriend()
                    || (fdes->getAccess() == clang::AS_public)) ;
            }
        }
    }
    return false ;
}

/** Prints namespace containers of a class delimited by delim */
void PrintFileContentsBase::printNamespaces( std::ostream & ostream , ConstructValues * c , const char * delim ) {
    for ( ClassValues::NamespaceIterator nsi = c->namespace_begin() ; nsi != c->namespace_end() ; ++nsi ) {
        ostream << *nsi << delim ;
    }
}

/** Prints namespace open block */
void PrintFileContentsBase::printOpenNamespaceBlocks( std::ostream & ostream , ClassValues * c ) {
    for ( ClassValues::NamespaceIterator nsi = c->namespace_begin() ; nsi != c->namespace_end() ; ++nsi ) {
        ostream << "namespace " << *nsi << " {\n" ;
    }
}

/** Prints namespace close block */
void PrintFileContentsBase::printCloseNamespaceBlocks( std::ostream & ostream , ClassValues * c ) {
    for ( ClassValues::NamespaceIterator nsi = c->namespace_begin() ; nsi != c->namespace_end() ; ++nsi ) {
        ostream << "}\n" ;
    }
}

/** Prints class containers of a class delimited by delim */
void PrintFileContentsBase::printContainerClasses( std::ostream & ostream , ConstructValues * c , const char * delim ) {
    for ( ClassValues::ContainerClassIterator ci = c->container_class_begin() ; ci != c->container_class_end() ; ci++ ) {
        ostream << *ci << delim ;
    }
}

