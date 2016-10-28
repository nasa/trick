
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

extern llvm::cl::opt< bool > global_compat15 ;

PrintFileContentsBase::PrintFileContentsBase() {}

// provide empty default implementation of these routines.
void PrintFileContentsBase::printClassMapHeader(std::ostream & ostream, std::string function_name ) {}
void PrintFileContentsBase::printClassMap(std::ostream & ostream, ClassValues * cv) {}
void PrintFileContentsBase::printClassMapFooter(std::ostream & ostream) {}

void PrintFileContentsBase::printEnumMapHeader(std::ostream & ostream, std::string function_name ) {}
void PrintFileContentsBase::printEnumMap(std::ostream & ostream, EnumValues * ev) {}
void PrintFileContentsBase::printEnumMapFooter(std::ostream & ostream) {}

void PrintFileContentsBase::print_units_map(std::ostream & ostream, ClassValues * cv ) {
    const std::string name = cv->getFullyQualifiedMangledTypeName("__");
    ostream << "struct UnitsMap" << name << " {\n" ;

    auto fields = getPrintableFields(*cv);
    if (fields.size()) {
        ostream << "    UnitsMap" << name << "() {\n"
                << "        Trick::UnitsMap* units_map_ptr = Trick::UnitsMap::units_map();\n" ;
        for (auto& field : fields) {
            ostream << "        units_map_ptr->add_param(\"" ;
            cv->printContainerClasses(ostream, "__");
            ostream << cv->getName() << "_" << field->getName() << "\", \"" << field->getUnits() << "\") ;\n" ;
        }
        ostream << "    }\n" ;
    }

    ostream << "} um" << name << ";\n" ;
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
        if ( global_compat15 or c->isCompat15()) {
            if ( fdes->isInherited() ) {
                return ((c->getHasInitAttrFriend() && fdes->getAccess() == clang::AS_protected)
                     || (fdes->getAccess() == clang::AS_public)) ;
            } else {
                return (c->getHasInitAttrFriend()
                    || (fdes->getAccess() == clang::AS_public)) ;
            }
        } else {
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
    }
    return false ;
}

std::vector<FieldDescription*> PrintFileContentsBase::getPrintableFields(ClassValues& classValues) {
    std::vector<FieldDescription*> results;
    for (auto& field : classValues.getFieldDescriptions()) {
        if (determinePrintAttr(&classValues, field) and field->getUnits().compare("1")) {
            results.push_back(field);
        }
    }
    return results;
}

