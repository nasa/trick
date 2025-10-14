
#include <iostream>
#include <sstream>
#include <algorithm>

#include "llvm/Support/CommandLine.h"
#include "EnumVisitor.hh"
#include "EnumValues.hh"
#include "Utilities.hh"
#include "BraceMacro.hh"

extern llvm::cl::opt< int > debug_level ;

EnumVisitor::EnumVisitor( clang::CompilerInstance & in_ci , HeaderSearchDirs & in_hsd ) :
 ci(in_ci) , hsd(in_hsd) {
    //eval = new EnumValues() ;
}

bool EnumVisitor::VisitDecl(clang::Decl *d) {
    //std::cout << "\n[34mEnum Decl = " << d->getDeclKindName() << "[00m" << std::endl ;
    return true ;
}

bool EnumVisitor::VisitType(clang::Type *t) {
    //std::cout << "\n[34mEnum Type = " << t->getTypeClassName() << "[00m" << std::endl ;
    return true;
}

bool EnumVisitor::VisitEnumDecl(clang::EnumDecl *ed) {
    eval.setFileName(getFileName(ci , ed->RBRACELOC(), hsd)) ;

    // Check if enum has an explicit underlying type (e.g., enum MY_ENUM : uint8_t)
    if (ed->isFixed()) {
        clang::QualType underlying_type = ed->getIntegerType();
        // Check if the underlying type is unsigned
        bool is_unsigned = underlying_type->isUnsignedIntegerType();
        eval.setIsUnsigned(is_unsigned);

        if ( debug_level >= 3 ) {
            std::string type_str = underlying_type.getAsString();
            std::cout << "EnumVisitor: Found enum with fixed underlying type: " 
                      << type_str << " (unsigned: " << is_unsigned << ")";

            // Show specific type detection
            if (underlying_type->isSpecificBuiltinType(clang::BuiltinType::UChar)) {
                std::cout << " [unsigned char/uint8_t]";
            } else if (underlying_type->isSpecificBuiltinType(clang::BuiltinType::UShort)) {
                std::cout << " [unsigned short/uint16_t]";
            } else if (underlying_type->isSpecificBuiltinType(clang::BuiltinType::UInt)) {
                std::cout << " [unsigned int/uint32_t]";
            } else if (underlying_type->isSpecificBuiltinType(clang::BuiltinType::ULong)) {
                std::cout << " [unsigned long]";
            } else if (underlying_type->isSpecificBuiltinType(clang::BuiltinType::ULongLong)) {
                std::cout << " [unsigned long long/uint64_t]";
            } else if (underlying_type->isSpecificBuiltinType(clang::BuiltinType::SChar)) {
                std::cout << " [signed char/int8_t]";
            } else if (underlying_type->isSpecificBuiltinType(clang::BuiltinType::Short)) {
                std::cout << " [short/int16_t]";
            } else if (underlying_type->isSpecificBuiltinType(clang::BuiltinType::Int)) {
                std::cout << " [int/int32_t]";
            } else if (underlying_type->isSpecificBuiltinType(clang::BuiltinType::Long)) {
                std::cout << " [long]";
            } else if (underlying_type->isSpecificBuiltinType(clang::BuiltinType::LongLong)) {
                std::cout << " [long long/int64_t]";
            }
            std::cout << std::endl;
        }
    } else {
        // No explicit underlying type, default to signed (traditional C enum behavior)
        eval.setIsUnsigned(false);

        if ( debug_level >= 3 ) {
            std::cout << "EnumVisitor: Found enum without fixed underlying type, defaulting to signed" << std::endl;
        }
    }

    return true;
}

bool EnumVisitor::VisitEnumType(clang::EnumType *et) {

    clang::TagDecl * td = et->getDecl() ;
    eval.setName(td->getName().str()) ;
    if ( debug_level >= 3 ) {
        std::cout << "EnumVisitor VisitEnumType " << std::endl ;
        std::cout << "eval.getName() = " << eval.getName() << std::endl ;
        std::cout << "td->getName() = " << td->getName().str() << std::endl ;
    }

    if ( eval.getName().empty() ) {
        std::string enum_type_name = et->desugar().getAsString() ;
        size_t pos ;
        // If this enumeration is anonymous return an error condition
        if ((pos = enum_type_name.find("<anonymous")) != std::string::npos or
            //(pos = enum_type_name.find("(anonymous")) != std::string::npos) {
            (pos = enum_type_name.find("(anonymous")) != std::string::npos or
            (pos = enum_type_name.find("(unnamed")) != std::string::npos) {
            eval.setName("") ;
            return false ;
        }
        if ((pos = enum_type_name.find("enum ")) != std::string::npos ) {
            enum_type_name.erase(pos , 5) ;
        }

        // Strip off containing namespace/class names.  They will be added back later.
        if ((pos = enum_type_name.find_last_of(":")) != std::string::npos ) {
            eval.setName(enum_type_name.substr(pos + 1)) ;
        } else {
            eval.setName(enum_type_name) ;
        }
        //std::cout << "\n[34mReplaced Enum name = " << eval->getName() << "[00m" << std::endl ;
    }

    eval.getNamespacesAndClasses(td->getDeclContext()) ;
    return true ;
}

bool EnumVisitor::VisitEnumConstantDecl(clang::EnumConstantDecl *ecd) {
    //std::cout << ecd->getName().str() << " = " << ecd->getInitVal().getSExtValue() << std::endl ;
    eval.addEnum(ecd->getName().str() , ecd->getInitVal().getSExtValue()) ;
    eval.addFullyQualifiedEnum(ecd->getQualifiedNameAsString(), ecd->getInitVal().getSExtValue());
    return true ;
}

EnumValues * EnumVisitor::get_enum_data() {
    return &eval ;
}


