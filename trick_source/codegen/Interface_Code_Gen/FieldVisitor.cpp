
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
        std::cout << "\n\033[32mFieldVisitor VisitDecl Decl = " << d->getDeclKindName() << "\033[00m" << std::endl ;
        d->dump() ;
    }
    return true ;
}

bool FieldVisitor::VisitType(clang::Type *t) {
    if ( debug_level >= 4 ) {
        std::cout << "FieldVisitor VisitType Type = " << t->getTypeClassName() << std::endl ;
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

    if ( debug_level >= 3 ) {
        std::cout << "FieldVisitor::VisitBuiltinType " << bt->desugar().getAsString() << std::endl ;
    }
    fdes->setTypeName(bt->desugar().getAsString()) ;
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
    return true;
}

bool FieldVisitor::VisitConstantArrayType(clang::ConstantArrayType *cat) {
    //cat->dump() ; std::cout << std::endl ;
    fdes->addArrayDim(cat->getSize().getZExtValue()) ;
    return true;
}

/* Both FieldDecl and VarDecl derive from DeclaratorDecl.  We can do
   common things to both node types in this function */
bool FieldVisitor::VisitDeclaratorDecl( clang::DeclaratorDecl *dd ) {

    fdes->setFileName(getFileName(ci , dd->getLocation(), hsd)) ;
    fdes->setName(dd->getNameAsString()) ;
    fdes->setAccess(dd->getAccess()) ;

    /* Get the source location of this field. */
    clang::SourceRange dd_range = dd->getSourceRange() ;
    std::string file_name = ci.getSourceManager().getBufferName(dd_range.getEnd()) ;
    char * resolved_path = almostRealPath( file_name.c_str() ) ;
    if ( resolved_path ) {
        if ( isInUserOrTrickCode( ci , dd_range.getEnd() , hsd ) ) {
            fdes->setLineNo(ci.getSourceManager().getSpellingLineNumber(dd_range.getEnd())) ;
            /* process comment if neither ICG:(No) or ICG:(NoComment) is present */
            if (  cs.hasTrickHeader(resolved_path) and
                 !cs.hasICGNoComment(resolved_path) and
                 !hsd.isPathInICGNoComment(resolved_path) ) {
                /* Get the possible comment on this line and parse it */
                fdes->parseComment(cs.getComment(resolved_path , fdes->getLineNo())) ;
            }
        }
        free(resolved_path) ;
    }

    if ( debug_level >= 3 ) {
        if ( ! ci.getSourceManager().isInSystemHeader(dd_range.getEnd()) ) {
            std::cout << "FieldVisitor VisitDeclaratorDecl" << std::endl ;
            //dd->dump() ;
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
    return true ;
}

bool FieldVisitor::VisitFieldDecl( clang::FieldDecl *field ) {
    fdes->setBitField(field->isBitField()) ;
    if ( fdes->isBitField() ) {
        fdes->setBitFieldWidth(field->getBitWidthValue(field->getASTContext())) ;
        unsigned int field_offset_bits = field->getASTContext().getFieldOffset(field) + fdes->getBaseClassOffset() * 8 ;
        fdes->setBitFieldStart( 32 - (field_offset_bits % 32) - fdes->getBitFieldWidth()) ;
        fdes->setBitFieldByteOffset((field_offset_bits / 32) * 4 ) ;
    }

    // If the current type is not canonical because of typedefs or template parameter substitution,
    // traverse the canonical type
    // Template type specializations are marked non canonical.
    // Do not process template specialization types here, allow normal processing to continue in that case.
    clang::QualType qt = field->getType() ;
    if ( debug_level >= 3 ) {
        std::cout << "FieldVisitor VisitFieldDecl" << std::endl ;
        std::cout << "    is_bitfield = " << fdes->isBitField() << std::endl ;
        std::cout << "    is_canonical = " << qt.isCanonical() << std::endl ;
        std::cout << "    is_a_template_specialization = " <<
         clang::TemplateSpecializationType::classof(qt.getTypePtr()) << std::endl ;
        //field->dump() ;
    }
    if ( !qt.isCanonical() and !clang::TemplateSpecializationType::classof(qt.getTypePtr()) ) {
        clang::QualType ct = qt.getCanonicalType() ;
        std::string tst_string = ct.getAsString() ;
        // Do not process standard template library specializations except strings
        if ( (! tst_string.compare( 0 , 9 , "class std") or ! tst_string.compare( 0 , 10 , "struct std")) and
               tst_string.compare( 0, 23 , "class std::basic_string") ) {
            // This is a standard template library type.  don't process any further.
            fdes->setIO(0) ;
        } else {
            if ( debug_level >= 3 ) {
                std::cout << "\033[33mFieldVisitor VisitFieldDecl: Processing canonical type\033[00m" << std::endl ;
                ct.dump() ;
            }
            TraverseType(ct) ;
        }
        // We have extracted the canonical type and everything else we need
        // return false so we cut off processing of this AST branch
        return false ;
    }

    return true ;
}

bool FieldVisitor::VisitPointerType(clang::PointerType *p) {
    fdes->addArrayDim(-1) ;
    return true;
}

bool FieldVisitor::VisitRecordType(clang::RecordType *rt) {
    if ( debug_level >= 3 ) {
        std::cout << "FieldVisitor VisitRecordType" << std::endl ;
        rt->dump() ;
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
                //fdes->setHasType(true) ;
                return false ;
            } else {
                //std::cout << "getDeclName " << type_name << std::endl ;
                fdes->setTypeName(type_name) ;
            }
        } else {
            //std::cout << "getTypedefNameForAnonDecl " << rt->getDecl()->getTypedefNameForAnonDecl() << std::endl ;
            fdes->setTypeName(rt->getDecl()->getTypedefNameForAnonDecl()->getQualifiedNameAsString()) ;
        }
    } else {
        // io_src code not possible for anonymous struct/unions.  Set the I/O to 0 to ignore it.
        if ( debug_level >= 3 ) {
            std::cout << "FieldVisitor VisitRecordType found anonymous type, setIO = 0" << std::endl ;
        }
        fdes->setIO(0) ;
    }

    fdes->setEnumString("TRICK_STRUCTURED") ;
    fdes->setRecord(true) ;
    // We have our type, return false to stop processing this AST branch
    return false;
}

std::map < std::string , std::string > processed_templates ;
/* This code handles handles class fields that define a new template type,
"my_var" in the below example.

template< class A>
class T {
    A a ;
} ;

class B {
    T<int> my_var ;
} ;
*/
bool FieldVisitor::VisitTemplateSpecializationType(clang::TemplateSpecializationType *tst) {
    std::string tst_string = tst->desugar().getAsString() ;
    if ( debug_level >= 3 ) {
        std::cout << "FieldVisitor VisitTemplateSpecializationType " << tst_string << std::endl ;
        //tst->dump() ;
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

                // Use the current template type name, tst_string, as part of the mangled type name
                // Some characters have to converted to underscores.
                std::string mangled_name = tst_string ;
                std::replace( mangled_name.begin(), mangled_name.end(), '<', '_') ;
                std::replace( mangled_name.begin(), mangled_name.end(), '>', '_') ;
                std::replace( mangled_name.begin(), mangled_name.end(), ' ', '_') ;
                std::replace( mangled_name.begin(), mangled_name.end(), ',', '_') ;
                std::replace( mangled_name.begin(), mangled_name.end(), ':', '_') ;
                std::replace( mangled_name.begin(), mangled_name.end(), '*', '_') ;

                CXXRecordVisitor template_spec_cvis(ci , cs, hsd , pa, false, false, true) ;
                template_spec_cvis.get_class_data()->setMangledTypeName(fdes->getContainerClass() + "_" +
                 fdes->getName() + "_" + mangled_name) ;
                template_spec_cvis.TraverseCXXRecordDecl(clang::cast<clang::CXXRecordDecl>(td)) ;
                fdes->setMangledTypeName(fdes->getContainerClass() + "_" + fdes->getName() + "_" + mangled_name) ;
                template_spec_cvis.get_class_data()->setName(tst_string) ;
                template_spec_cvis.get_class_data()->setFileName(fdes->getFileName()) ;

                pa.printClass(template_spec_cvis.get_class_data()) ;

                // save off the mangled name of this template to be used if another variable is the same template type
                processed_templates[tst_string] = template_spec_cvis.get_class_data()->getMangledTypeName() ;

                fdes->setEnumString("TRICK_STRUCTURED") ;
                fdes->setRecord(true) ;
                if ( debug_level >= 4 ) {
                    std::cout << "Added template class from FieldVisitor VisitTemplateSpecializationType " ;
                    std::cout << tst_string << std::endl ;
                    std::cout << *fdes << std::endl ;
                }
            } else {
                // reuse the name of the template already processed.
                fdes->setMangledTypeName(processed_templates[tst_string]) ;
                fdes->setEnumString("TRICK_STRUCTURED") ;
                fdes->setRecord(true) ;
            }
        }
    }
    // We have either extracted the template type or have marked this field I/O = 0.
    // We can return false to stop processing of this AST branch
    return false ;
}

bool FieldVisitor::VisitVarDecl( clang::VarDecl *v ) {
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
    if ( debug_level >= 3 ) {
        std::cout << "FieldVisitor VisitVarDecl " << fdes->getName() << std::endl ;
        std::cout << "    is static = " << fdes->isStatic() << std::endl ;
        std::cout << "    is const = " << v->getType().isConstQualified() << std::endl ;
        std::cout << "    has initializer value = " << v->hasInit() << std::endl ;
        std::cout << "    IO = " << fdes->getIO() << std::endl ;
        //v->dump() ; std::cout << std::endl ;
    }
    return true ;
}

FieldDescription * FieldVisitor::get_field_data() {
    return fdes ;
}

