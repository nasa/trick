
#include <iostream>

#include "llvm/Support/CommandLine.h"
#include "clang/Basic/SourceManager.h"
#include "clang/AST/RecordLayout.h"
#include "clang/AST/ASTContext.h"

#include "FieldVisitor.hh"
#include "FieldDescription.hh"
#include "ClassValues.hh"
#include "EnumValues.hh"
#include "ClassVisitor.hh"
#include "CommentSaver.hh"
#include "Utilities.hh"

extern llvm::cl::opt< int > debug_level ;

FieldVisitor::FieldVisitor(clang::CompilerInstance & in_ci ,
 HeaderSearchDirs & in_hsd ,
 CommentSaver & in_cs ,
 PrintAttributes & in_pa ,
 std::string container_class ,
 bool in_access_spec_found ,
 bool in_inherited ,
 bool in_virtual_inherited ,
 unsigned int in_base_class_offset ) :
  ci(in_ci) ,
  hsd(in_hsd) ,
  cs(in_cs) ,
  pa(in_pa) {
    fdes = new FieldDescription(container_class, in_access_spec_found, in_inherited, in_virtual_inherited, in_base_class_offset) ;
}

bool FieldVisitor::VisitDecl(clang::Decl *d) {
    if ( debug_level >= 4 ) {
        std::cout << "\nFieldVisitor Decl = " << d->getDeclKindName() << std::endl ;
        d->dump() ;
    }
    return true ;
}

bool FieldVisitor::VisitType(clang::Type *t) {
    if ( debug_level >= 4 ) {
        std::cout << "\nFieldVisitor Type = " << t->getTypeClassName() << std::endl ;
        t->dump() ;
    }
    // If this type is a reference, set IO to 0
    if ( t->isReferenceType() ) {
        if ( debug_level >= 3 ) {
            std::cout << "FieldVisitor VisitType found reference, setIO = 0 " << std::endl ;
        }
        fdes->setIO(0) ;
    }
    return true;
}

bool FieldVisitor::VisitBuiltinType(clang::BuiltinType *bt) {

    fdes->setTypeName(bt->desugar().getAsString()) ;
    if ( debug_level >= 3 ) {
        std::cout << "FieldVisitor::VisitBuiltinType " << fdes->getTypeName() << std::endl ;
    }
    if ( fdes->isBitField() ) {
        if ( bt->isUnsignedInteger() ) {
            fdes->setEnumString("TRICK_UNSIGNED_BITFIELD") ;
        } else {
            fdes->setEnumString("TRICK_BITFIELD") ;
        }
        if ( bt->getKind() == clang::BuiltinType::Bool ) {
            fdes->setTypeName("bool") ;
        }
    } else {
        switch ( bt->getKind() ) {
            case clang::BuiltinType::Bool:
                fdes->setEnumString("TRICK_BOOLEAN") ;
                fdes->setTypeName("bool") ;
                break ;
            case clang::BuiltinType::Char_S:
            case clang::BuiltinType::SChar:
                fdes->setEnumString("TRICK_CHARACTER") ;
                break ;
            case clang::BuiltinType::UChar:
            case clang::BuiltinType::Char_U:
                fdes->setEnumString("TRICK_UNSIGNED_CHARACTER") ;
                break ;
            case clang::BuiltinType::WChar_U:
            case clang::BuiltinType::WChar_S:
                fdes->setEnumString("TRICK_WCHAR") ;
                break ;
            case clang::BuiltinType::Short:
                fdes->setEnumString("TRICK_SHORT") ;
                break ;
            case clang::BuiltinType::UShort:
            case clang::BuiltinType::Char16:
                fdes->setEnumString("TRICK_UNSIGNED_SHORT") ;
                break ;
            case clang::BuiltinType::Int:
                fdes->setEnumString("TRICK_INTEGER") ;
                break ;
            case clang::BuiltinType::UInt:
                fdes->setEnumString("TRICK_UNSIGNED_INTEGER") ;
                break ;
            case clang::BuiltinType::Long:
                fdes->setEnumString("TRICK_LONG") ;
                break ;
            case clang::BuiltinType::ULong:
                fdes->setEnumString("TRICK_UNSIGNED_LONG") ;
                break ;
            case clang::BuiltinType::LongLong:
                fdes->setEnumString("TRICK_LONG_LONG") ;
                break ;
            case clang::BuiltinType::ULongLong:
                fdes->setEnumString("TRICK_UNSIGNED_LONG_LONG") ;
                break ;
            case clang::BuiltinType::Float:
                fdes->setEnumString("TRICK_FLOAT") ;
                break ;
            case clang::BuiltinType::Double:
                fdes->setEnumString("TRICK_DOUBLE") ;
                break ;
            default:
                fdes->setEnumString("TRICK_VOID") ;
                break ;
        }
    }
    fdes->setHasType(true) ;
    return true;
}

bool FieldVisitor::VisitConstantArrayType(clang::ConstantArrayType *cat) {
    //cat->dump() ; std::cout << std::endl ;
    if ( ! fdes->hasDims() ) {
        fdes->addArrayDim(cat->getSize().getZExtValue()) ;
    }
    return true;
}

/* Both FieldDecl and VarDecl derive from DeclaratorDecl.  We can do
   common things to both node types in this function */
bool FieldVisitor::VisitDeclaratorDecl( clang::DeclaratorDecl *dd ) {

    fdes->setFileName(getFileName(ci , dd->getLocation(), hsd)) ;
    fdes->setName(dd->getNameAsString()) ;
    fdes->setAccess(dd->getAccess()) ;

    /* Get the source location of this field.*/
    clang::SourceRange dd_range = dd->getSourceRange() ;
    clang::PresumedLoc PLoc = ci.getSourceManager().getPresumedLoc(dd_range.getEnd());
    std::string file_name ;
    if (!PLoc.isInvalid()) {
        char * resolved_path = almostRealPath(PLoc.getFilename()) ;
        if ( resolved_path != NULL ) {
            file_name = std::string(resolved_path) ;
            free(resolved_path) ;
        }
    }

    if ( ! file_name.empty() ) {
        if ( isInUserOrTrickCode( ci , dd_range.getEnd() , hsd ) ) {
            fdes->setLineNo(ci.getSourceManager().getSpellingLineNumber(dd_range.getEnd())) ;
            /* process comment if neither ICG:(No) or ICG:(NoComment) is present */
            if (  cs.hasTrickHeader(file_name) and
                 !cs.hasICGNoComment(file_name) and
                 !hsd.isPathInICGNoComment(file_name) ) {
                /* Get the possible comment on this line and parse it */
                fdes->parseComment(cs.getComment(file_name , fdes->getLineNo())) ;
            }
        }
    }

    if ( debug_level >= 3 ) {
        if ( ! ci.getSourceManager().isInSystemHeader(dd_range.getEnd()) ) {
            std::cout << "\nFieldVisitor VisitDeclaratorDecl" << std::endl ;
            dd->dump() ; std::cout << std::endl ;
            std::cout << "    public/private = " << fdes->getAccess() << std::endl ;
        }
    }

    return true ;
}

bool FieldVisitor::VisitEnumType( clang::EnumType *et ) {

    std::string enum_type_name = et->desugar().getAsString() ;
    if ( debug_level >= 3 ) {
        std::cout << "\nFieldVisitor VisitEnumType" << std::endl ;
        std::cout << et->desugar().getAsString() << std::endl ;
    }
    size_t pos ;
    if ((pos = enum_type_name.find("enum ")) != std::string::npos ) {
        enum_type_name.erase(pos , 5) ;
    }
    // If this enum is to an enumeration found inside a template, e.g. template<type>::enum_type ignore it.
    // because there will not be enumeration attribute information generated for this enum.
    if ((pos = enum_type_name.find("<")) != std::string::npos ) {
        size_t last_pos = enum_type_name.find_last_of(">::") ;
        enum_type_name.replace(pos, last_pos - pos + 1, "__") ;
        //fdes->setIO(0) ;
    }
    fdes->setMangledTypeName("") ;
    fdes->setTypeName(enum_type_name) ;
    fdes->setEnumString("TRICK_ENUMERATED") ;
    fdes->setEnum(true) ;
    fdes->setHasType(true) ;
    return true ;
}

bool FieldVisitor::VisitFieldDecl( clang::FieldDecl *field ) {
    if ( debug_level >= 3 ) {
        std::cout << "\nFieldVisitor VisitFieldDecl" << std::endl ;
        field->dump() ; std::cout << std::endl ;
    }
    fdes->setBitField(field->isBitField()) ;
    if ( fdes->isBitField() ) {
        fdes->setBitFieldWidth(field->getBitWidthValue(field->getASTContext())) ;
        unsigned int field_offset_bits = field->getASTContext().getFieldOffset(field) + fdes->getBaseClassOffset() * 8 ;
        fdes->setBitFieldStart( 32 - (field_offset_bits % 32) - fdes->getBitFieldWidth()) ;
        fdes->setBitFieldByteOffset((field_offset_bits / 32) * 4 ) ;
    }

    clang::QualType qt = field->getType() ;
    // If the current type is not canonical because of typedefs, traverse the canonical type
    if ( !qt.isCanonical() ) {
        clang::QualType ct = qt.getCanonicalType() ;
        TraverseType(ct) ;
        // Traversing the canonical type will set the has_type flag.  Set the has_dims flag too so
        // when we continue parsing the AST and the non-canonical type, we don't double add dimensions.
        fdes->setHasDims(true) ;
    }
    return true ;
}

bool FieldVisitor::VisitElaboratedType(clang::ElaboratedType *et) {
    if ( debug_level >= 4 ) {
        std::cout << "FieldVisitor VisitElaboratedType" << std::endl ;
    }
    //et->dump() ; std::cout << std::endl ;
    return true ;
}

bool FieldVisitor::VisitPointerType(clang::PointerType *p) {
    if ( ! fdes->hasDims() ) {
        fdes->addArrayDim(-1) ;
    }
    return true;
}

bool FieldVisitor::VisitRecordType(clang::RecordType *rt) {
    if ( debug_level >= 3 ) {
        std::cout << "FieldVisitor VisitRecordType" << std::endl ;
        rt->dump() ; std::cout << std::endl ;
    }
    /* An array parameter may be a value within a record type.  In this case we
       may end up here multiple times for a field.  The first
       visitor is field type.  Other visits are for the array parameters.
       Return if we have already set the type for this field. */
    if ( fdes->hasType() ) {
        return true ;
    }
    /* Test to see if we have an embedded anonymous struct/union.  e.g. SB is anonymous below.
       struct SA {
          struct {
             double d ;
          } SB ;
       } ;
    */
    //std::cout << "hasNameForLinkage " << rt->getDecl()->hasNameForLinkage() << std::endl ;
    if ( rt->getDecl()->hasNameForLinkage() ) {
        if ( rt->getDecl()->getDeclName() ) {
            std::string type_name = rt->getDecl()->getQualifiedNameAsString() ;
            // Handle the string class differently than regular records.
            if ( ! type_name.compare("std::basic_string") || !type_name.compare("std::__1::basic_string")) { 
                fdes->setEnumString("TRICK_STRING") ;
                fdes->setTypeName("std::string") ;
                fdes->setHasType(true) ;
                return true ;
            } else {
                //std::cout << "getDeclName " << type_name << std::endl ;
                fdes->setTypeName(type_name) ;
            }
        } else {
            //std::cout << "getTypedefNameForAnonDecl " << rt->getDecl()->getTypedefNameForAnonDecl() << std::endl ;
            fdes->setTypeName(rt->getDecl()->getTypedefNameForAnonDecl()->getQualifiedNameAsString()) ;
        }
    } else {
        //rt->getDecl()->dump() ; std::cout << std::endl ;
        // io_src code not possible for anonymous struct/unions.  Set the I/O to 0 to ignore it.
        if ( debug_level >= 3 ) {
            std::cout << "FieldVisitor VisitRecordType found anonymous type, setIO = 0" << std::endl ;
        }
        fdes->setIO(0) ;
    }

    fdes->setEnumString("TRICK_STRUCTURED") ;
    fdes->setRecord(true) ;
    fdes->setHasType(true) ;
    return true;
}

/*
    In a template, this is the type that is defined by one of the incoming parameters.
    template < class T > class A {
        T var ;
    } ;
    In the above example this visitor is the resolved "T" class on the line "T var ;"
 */
bool FieldVisitor::VisitSubstTemplateTypeParmType(clang::SubstTemplateTypeParmType *sttpt) {
    if ( debug_level >= 3 ) {
        std::cout << "FieldVisitor VisitSubstTemplateTypeParmType" << std::endl ;
        sttpt->dump() ; std::cout << std::endl ;
    }
    if ( fdes->hasType() ) {
        if ( debug_level >= 3 ) {
            std::cout << "FieldVisitor VisitSubstTemplateTypeParmType field has type already" << std::endl ;
        }
        return true ;
    }
    if ( sttpt->isBuiltinType() ) {
        VisitBuiltinType((clang::BuiltinType *)sttpt->getAs<clang::BuiltinType>()) ;
    } else if ( sttpt->isEnumeralType() ) {
        VisitEnumType((clang::EnumType *)sttpt->getAs<clang::EnumType>()) ;
    } else if ( sttpt->isRecordType() ) {
        VisitRecordType((clang::RecordType *)sttpt->getAs<clang::RecordType>()) ;
    }
    return true;
}

std::map < std::string , ClassValues * > processed_templates ;
/* This code handles handles class fields that define a new template type,
"my_var" in the below example.

template< class A>
class T {
    A a ;
} ;

class B {
    T<int> my_var ;
} ;

TODO:  We need to save all of the parts of the template separately.  There is code in
       PrintFilecontents10 that I want to fix, but I would need the template info stored differently.
*/
bool FieldVisitor::VisitTemplateSpecializationType(clang::TemplateSpecializationType *tst) {
    std::string tst_string = tst->desugar().getAsString() ;
    if ( debug_level >= 3 ) {
        std::cout << "FieldVisitor VisitTemplateSpecializationType " << tst_string << std::endl ;
        tst->dump() ; std::cout << std::endl ;
    }

    if ( ! tst_string.compare( 0 , 9 , "class std") or ! tst_string.compare( 0 , 10 , "struct std") ) {
        // This is a standard template library type.  Set the io field to 0.
        fdes->setIO(0) ;
    } else if ( ! tst_string.compare( 0 , 11 , "class Trick") or ! tst_string.compare( 0 , 12 , "struct Trick") ) {
        // This is a Trick standard template library type.  Set the io field to 0.
        fdes->setIO(0) ;
    } else if ( fdes->getIO() != 0 ) {
        // Continue processing only if we haven't set the IO spec to 0.
        // This is a user template type. We'll need the expanded template class definition
        clang::CXXRecordDecl *trec = tst->getAsCXXRecordDecl() ;

        // If we cannot get a definition, ignore this parameter and return with no further processing.
        if ( trec == NULL ) {
            fdes->setIO(0) ;
            return false ;
        }

        clang::TagDecl * td = trec->getDefinition() ;

        if ( td != NULL ) {
            size_t pos ;
            if ( debug_level >= 3 ) {
                std::cout << "FieldVisitor VisitTemplateSpecializationType CXXRecordDecl " << std::endl ;
                td->dump() ; std::cout << std::endl ;
            }
            if ((pos = tst_string.find("class ")) != std::string::npos ) {
                tst_string.erase(pos , 6) ;
            }
            // clang changes bool to _Bool.  We need to change it back
            if ((pos = tst_string.find("<_Bool")) != std::string::npos ) {
                tst_string.replace(pos , 6, "<bool") ;
            }
            while ((pos = tst_string.find(" _Bool")) != std::string::npos ) {
                tst_string.replace(pos , 6, " bool") ;
            }
            // NOTE: clang also changes FILE * to struct _SFILE *.  We may need to change that too.

            // Save container namespaces and classes.
            fdes->getNamespacesAndClasses(td->getDeclContext()) ;

            // Check to see if we've created a class that matches this template
            if ( processed_templates.find(tst_string) == processed_templates.end() ) {
                CXXRecordVisitor template_spec_cvis(ci , cs, hsd , pa, false, false, true) ;
                processed_templates.insert(std::pair< std::string , ClassValues * >(tst_string , template_spec_cvis.get_class_data())) ;

                // Use the current template type name, tst_string, as part of the mangled type name
                // Some characters have to converted to underscores.
                std::string mangled_name = tst_string ;
                std::replace( mangled_name.begin(), mangled_name.end(), '<', '_') ;
                std::replace( mangled_name.begin(), mangled_name.end(), '>', '_') ;
                std::replace( mangled_name.begin(), mangled_name.end(), ' ', '_') ;
                std::replace( mangled_name.begin(), mangled_name.end(), ',', '_') ;
                std::replace( mangled_name.begin(), mangled_name.end(), ':', '_') ;
                std::replace( mangled_name.begin(), mangled_name.end(), '*', '_') ;


                template_spec_cvis.get_class_data()->setMangledTypeName(fdes->getContainerClass() + "_" +
                 fdes->getName() + "_" + mangled_name) ;
                template_spec_cvis.TraverseCXXRecordDecl(clang::cast<clang::CXXRecordDecl>(td)) ;
                fdes->setMangledTypeName(fdes->getContainerClass() + "_" + fdes->getName() + "_" + mangled_name) ;
                template_spec_cvis.get_class_data()->setName(tst_string) ;
                template_spec_cvis.get_class_data()->setFileName(fdes->getFileName()) ;

                pa.printClass(template_spec_cvis.get_class_data()) ;

                // template_spec_cvis is going to go out of scope.  We need to make a copy of the ClassValues.
                ClassValues * cval = new ClassValues(*(template_spec_cvis.get_class_data())) ;
                processed_templates[tst_string] = cval ;

                fdes->setEnumString("TRICK_STRUCTURED") ;
                fdes->setRecord(true) ;
                fdes->setHasType(true) ;
                if ( debug_level >= 4 ) {
                    std::cout << "Added template class from FieldVisitor VisitTemplateSpecializationType " ;
                    std::cout << tst_string << std::endl ;
                    std::cout << *fdes << std::endl ;
                }
            } else {
                ClassValues * cval = processed_templates[tst_string] ;
                fdes->setMangledTypeName(cval->getMangledTypeName()) ;
                fdes->setEnumString("TRICK_STRUCTURED") ;
                fdes->setRecord(true) ;
                fdes->setHasType(true) ;
            }
        }
    }
    return true ;
}

bool FieldVisitor::VisitTemplateTypeParmType(clang::TemplateTypeParmType *ttp) {
    fdes->setTypeName(ttp->getDecl()->getNameAsString()) ;
    return true ;
}

class IgnoreTypes {
    public:
        IgnoreTypes() {
           types.insert("basic_ostringstream<char>") ;
        }
        std::set< std::string > types ;
} ;
IgnoreTypes ignore_types ;

bool FieldVisitor::VisitTypedefType(clang::TypedefType *tt) {
    size_t pos ;

    if ( debug_level >= 3 ) {
        std::cout << "FieldVisitor VisitTypedefType " << fdes->getName() << std::endl ;
        tt->dump() ; std::cout << std::endl ;
    }

    // return if we have set a typename
    if ( fdes->hasType() ) {
        return true ;
    }

    if ( tt->isEnumeralType() ) {
        fdes->setEnumString("TRICK_ENUMERATED") ;
        fdes->setEnum(true) ;
        std::string enum_type_name = tt->desugar().getAsString() ;

        if ((pos = enum_type_name.find("enum ")) != std::string::npos ) {
            enum_type_name.erase(pos , 5) ;
        }

        //std::cout << "enum name " << enum_type_name << std::endl ;
        fdes->setTypeName(enum_type_name) ;

        // Save container namespaces and classes.
        const clang::TypedefNameDecl * tnd = tt->getDecl() ;
        fdes->getNamespacesAndClasses(tnd->getDeclContext()) ;

        fdes->setHasType(true) ;
    } else if ( tt->isRecordType() ) {

        if ( debug_level >= 4 ) {
            std::cout << "FieldVisitor isRecordType" << std::endl ;
        }

        std::string type_name = tt->desugar().getAsString() ;
        if ((pos = type_name.find("struct ")) != std::string::npos ) {
            type_name.erase(pos , 7) ;
        }
        if ((pos = type_name.find("union ")) != std::string::npos ) {
            type_name.erase(pos , 6) ;
        }
        // Typedefs of STL containers are ignored.
        if ((pos = type_name.find("std::")) != std::string::npos ) {
            fdes->setIO(0) ;
            return true ;
        } else if (ignore_types.types.find(type_name) != ignore_types.types.end() ) {
            // Ignore other system types that do not start with std::
            fdes->setIO(0) ;
            return true ;
        }

        if ( ! type_name.compare(0, 17, "basic_string<char") ) {
            fdes->setEnumString("TRICK_STRING") ;
            type_name = "std::string";
        } else if ( ! type_name.compare("_IO_FILE") ) {
            fdes->setEnumString("TRICK_FILE_PTR") ;
        } else if ( ! type_name.compare("__sFILE") ) {
            fdes->setEnumString("TRICK_FILE_PTR") ;
        } else {
            fdes->setEnumString("TRICK_STRUCTURED") ;
            fdes->setRecord(true) ;

            const clang::TypedefNameDecl * tnd = tt->getDecl() ;
            fdes->getNamespacesAndClasses(tnd->getDeclContext()) ;
            // If this typedef resolves to a class/template in std namespace, don't process it.
            if ( fdes->namespace_begin() != fdes->namespace_end() ) {
                std::string first_ns = *(fdes->namespace_begin()) ;
                if ( ! first_ns.compare("std") ) {
                    fdes->setIO(0) ;
                    return true ;
                }
            }

            // test to see if the typedef name is a template type we have processed.
            std::map < std::string , ClassValues * >::iterator mit ;
            mit = processed_templates.find(type_name) ;
            if ( mit != processed_templates.end() ) {
                if ( debug_level >= 4 ) {
                    std::cout << "templated type has been processed before " << (*mit).second->getMangledTypeName() << std::endl ;
                }
                type_name = (*mit).second->getMangledTypeName() ;
            } else {
                // TODO: Is there a better way to determine if this is a template or not?  Right now we're
                // checking if the typename has a "<" character.
                if ( type_name.find("<") != std::string::npos ) {
                    clang::CXXRecordDecl *trec = tt->getAsCXXRecordDecl() ;
                    unsigned int before_enum_size ;
                    if ( debug_level >= 4 ) {
                        std::cout << "templated type unprocessed! " << type_name << std::endl ;
                        trec->dump() ; std::cout << std::endl ;
                    }

                    CXXRecordVisitor template_spec_cvis(ci , cs, hsd , pa, false, false, true) ;
                    template_spec_cvis.get_class_data()->setMangledTypeName(fdes->getName() + "_" +
                     clang::cast<clang::CXXRecordDecl>(trec)->getNameAsString()) ;
                    template_spec_cvis.TraverseCXXRecordDecl(trec) ;
                    fdes->setMangledTypeName(fdes->getName() + "_" + template_spec_cvis.get_class_data()->getName()) ;
                    template_spec_cvis.get_class_data()->setName(type_name) ;
                    template_spec_cvis.get_class_data()->setFileName(fdes->getFileName()) ;

                    ClassValues * cval = new ClassValues(*(template_spec_cvis.get_class_data())) ;
                    processed_templates.insert(std::pair< std::string , ClassValues * >(type_name , cval)) ;
                    pa.printClass(template_spec_cvis.get_class_data()) ;

                    fdes->setEnumString("TRICK_STRUCTURED") ;
                    fdes->setRecord(true) ;
                    fdes->setHasType(true) ;
                    if ( debug_level >= 4 ) {
                        std::cout << "Added template class from typedeffed field " << type_name << std::endl ;
                        std::cout << "  fdes->getMangledTypeName " << fdes->getMangledTypeName() << std::endl ;
                        std::cout << *cval << std::endl ;
                    }
                }
            }

        }
        fdes->setTypeName(type_name) ;
        fdes->setHasType(true) ;
    } else if ( tt->isBuiltinType() ) {
        const clang::BuiltinType * bt = tt->getAs<clang::BuiltinType>() ;
        VisitBuiltinType((clang::BuiltinType *)bt) ;
    }
    return true;
}

bool FieldVisitor::VisitVarDecl( clang::VarDecl *v ) {
    if ( debug_level >= 3 ) {
        std::cout << "FieldVisitor VisitVarDecl " << fdes->getName() << std::endl ;
        v->dump() ; std::cout << std::endl ;
    }
    fdes->setStatic(v->isStaticDataMember()) ;
    /* If we have a static const integer type with an initializer value, this variable will
       not be instantiated by the compiler. The compiler substitutes in the value internally.
       set the IO to 0 to stop attribute printing */
    // Note: gcc allows an initializer for floating point types too.
    if ( v->isStaticDataMember() and
         v->getType().isConstQualified() and
         v->hasInit() ) {
        fdes->setIO(0) ;
    } else if ( v->isStaticDataMember() and
         v->getType().isConstQualified() ) {
        /* Static const members cannot be set through attributes code. Remove input
           capabilities by taking current io specification & 1 */
        fdes->setIO(fdes->getIO() & 1) ;
    }
    return true ;
}

FieldDescription * FieldVisitor::get_field_data() {
    return fdes ;
}

