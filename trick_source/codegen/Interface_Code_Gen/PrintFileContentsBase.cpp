
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <libgen.h>
#include <string.h>
#include <iomanip>

#include "PrintFileContentsBase.hh"
#include "FieldDescription.hh"
#include "ClassValues.hh"
#include "EnumValues.hh"

PrintFileContentsBase::PrintFileContentsBase() {}

// provide empty default implementation of these routines.
void PrintFileContentsBase::printClassMapHeader(std::ofstream & out, std::string function_name ) {}
void PrintFileContentsBase::printClassMap(std::ofstream & out, ClassValues * cv) {}
void PrintFileContentsBase::printClassMapFooter(std::ofstream & out) {}

void PrintFileContentsBase::printEnumMapHeader(std::ofstream & out, std::string function_name ) {}
void PrintFileContentsBase::printEnumMap(std::ofstream & out, EnumValues * ev) {}
void PrintFileContentsBase::printEnumMapFooter(std::ofstream & out) {}

void PrintFileContentsBase::print_units_map(std::ofstream & outfile, ClassValues * cv ) {
    ClassValues::FieldIterator fit ;
    unsigned int ii ;

    outfile << "struct UnitsMap" ;
    printNamespaces( outfile, cv , "__" ) ;
    printContainerClasses( outfile, cv , "__" ) ;
    outfile << cv->getMangledTypeName() ;
    outfile << " {\n" ;
    outfile << "    UnitsMap" ;
    printNamespaces( outfile, cv , "__" ) ;
    printContainerClasses( outfile, cv , "__" ) ;
    outfile << cv->getMangledTypeName() ;
    outfile << "() {\n" ;

    outfile << "        Trick::UnitsMap * units_map_ptr __attribute__((unused)) = Trick::UnitsMap::units_map() ;\n" ;
    for ( fit = cv->field_begin() ; fit != cv->field_end() ; fit++ ) {
        if ( determinePrintAttr(cv , *fit) and (*fit)->getUnits().compare("1")) {
            FieldDescription * fdes = *fit ;
            outfile << "        units_map_ptr->add_param(\"" ;
            printContainerClasses( outfile, cv , "__" ) ;
            outfile << cv->getName() << "_" << fdes->getName() << "\", \"" << fdes->getUnits() << "\") ;\n" ;
        }
    }
    outfile << "    }\n" ;
    outfile << "} ;\n\n" ;

    outfile << "UnitsMap" ;
    printNamespaces( outfile, cv , "__" ) ;
    printContainerClasses( outfile, cv , "__" ) ;
    outfile << cv->getMangledTypeName() ;
    outfile << " um" ;
    printNamespaces( outfile, cv , "__" ) ;
    printContainerClasses( outfile, cv , "__" ) ;
    outfile << cv->getMangledTypeName() ;
    outfile << " ;\n\n" ;
}

/* Utility routines for printing */
void PrintFileContentsBase::print_open_extern_c(std::ofstream & outfile) {
    outfile << "extern \"C\" {\n\n" ;
}

void PrintFileContentsBase::print_close_extern_c(std::ofstream & outfile) {
    outfile << "\n} //extern \"C\"\n\n" ;
}

/* internal function determines if a particular field is printable based
   on access to the field and the presense of init_attr friends.
*/
bool PrintFileContentsBase::determinePrintAttr( ClassValues * c , FieldDescription * fdes ) {
    if ( fdes->getTypeName().compare("void") and fdes->getIO() != 0 and fdes->getEnumString().compare("TRICK_VOID")) {
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
    }
    return false ;
}

/** Prints namespace containers of a class delimited by delim */
void PrintFileContentsBase::printNamespaces( std::ofstream & outfile , ConstructValues * c , const char * delim ) {
    ClassValues::NamespaceIterator nsi ;
    for ( nsi = c->namespace_begin() ; nsi != c->namespace_end() ; nsi++ ) {
        outfile << *nsi << delim ;
    }
}

/** Prints namespace open block */
void PrintFileContentsBase::printOpenNamespaceBlocks( std::ofstream & outfile , ClassValues * c ) {
    ClassValues::NamespaceIterator nsi ;
    for ( nsi = c->namespace_begin() ; nsi != c->namespace_end() ; nsi++ ) {
        outfile << "namespace " << *nsi << " {\n" ;
    }
}

/** Prints namespace close block */
void PrintFileContentsBase::printCloseNamespaceBlocks( std::ofstream & outfile , ClassValues * c ) {
    ClassValues::NamespaceIterator nsi ;
    for ( nsi = c->namespace_begin() ; nsi != c->namespace_end() ; nsi++ ) {
        outfile << "}\n" ;
    }
}

/** Prints class containers of a class delimited by delim */
void PrintFileContentsBase::printContainerClasses( std::ofstream & outfile , ConstructValues * c , const char * delim ) {
    ClassValues::ContainerClassIterator ci ;
    for ( ci = c->container_class_begin() ; ci != c->container_class_end() ; ci++ ) {
        outfile << *ci << delim ;
    }
}

