
#include <iostream>
#include <sstream>

#include "llvm/Support/CommandLine.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/FileManager.h"
#include "clang/AST/RecordLayout.h"
#include "clang/AST/ASTContext.h"

#include "ClassVisitor.hh"
#include "EnumVisitor.hh"
#include "EnumValues.hh"
#include "FieldVisitor.hh"
#include "Utilities.hh"
#include "PrintAttributes.hh"

extern llvm::cl::opt< int > debug_level ;

CXXRecordVisitor::CXXRecordVisitor(
 clang::CompilerInstance & in_ci ,
 CommentSaver & in_cs ,
 HeaderSearchDirs & in_hsd ,
 PrintAttributes & in_pa ,
 bool in_inherited ,
 bool in_virtual_inherited ,
 bool in_include_virtual_base ,
 unsigned int in_base_class_offset) :
  ci(in_ci) ,
  cs(in_cs) ,
  hsd(in_hsd) ,
  pa(in_pa) ,
  cval(in_inherited , in_virtual_inherited) ,
  include_virtual_base(in_include_virtual_base) ,
  base_class_offset(in_base_class_offset) ,
  access_spec_found(false)
 {
    //cval = new ClassValues(in_inherited , in_virtual_inherited) ;
}

CXXRecordVisitor::~CXXRecordVisitor() {
}

bool CXXRecordVisitor::TraverseDecl(clang::Decl *d) {
    if (!d)
        return true;
    if (d->isImplicit())
        return true;

    //std::cout << "CV " << d->getDeclKindName() << std::endl ;

    switch ( d->getKind() ) {
        case clang::Decl::ClassTemplateSpecialization : {
            TraverseClassTemplateSpecializationDecl(static_cast<clang::ClassTemplateSpecializationDecl *>(d)) ;
        }
        break ;
        case clang::Decl::CXXRecord : {
            clang::CXXRecordDecl * crd = static_cast<clang::CXXRecordDecl *>(d) ;
            /* The definition of the record must exist before we can process it.  The definition is
               NULL when this is only a forward declaration of a class.  We also only want to
               process embedded classes that have public access. */
            clang::RecordDecl * rd = crd->getDefinition() ;
            if ( rd != NULL and rd->getAccess() == clang::AS_public ) {
                if ( isInUserCode(ci , crd->getSourceRange().getEnd(), hsd) ) {
                    CXXRecordVisitor embedded_cvis(ci , cs, hsd , pa, false, false, true) ;
                    embedded_cvis.TraverseCXXRecordDecl(static_cast<clang::CXXRecordDecl *>(d)) ;
                    pa.printClass(embedded_cvis.get_class_data()) ;
                }
            }
        }
        break ;
        case clang::Decl::Enum : {
            clang::EnumDecl * ed = static_cast<clang::EnumDecl *>(d) ;
            /* This is an embedded enumeration within a class.  Only process it if is public */
            if ( ed->getAccess() == clang::AS_public ) {
                EnumVisitor evis(ci, hsd) ;
                evis.TraverseDecl(ed) ;
                //if ( evis.get_enum_data() != NULL ) {
                    pa.printEnum(evis.get_enum_data()) ;
                //}
            }
        }
        break ;
        case clang::Decl::Field : {
            FieldVisitor fvis(ci , hsd , cs, pa, cval.getName() , access_spec_found , cval.isInherited(), cval.isVirtualInherited(), base_class_offset) ;
            fvis.TraverseFieldDecl(static_cast<clang::FieldDecl *>(d)) ;
            cval.addFieldDescription(fvis.get_field_data()) ;
        }
        break ;
        case clang::Decl::Friend : {
            TraverseFriendDecl(static_cast<clang::FriendDecl *>(d)) ;
        }
        break ;
        case clang::Decl::Var : {
            /* Static fields appear as vars. Treat it as a field. */
            FieldVisitor fvis(ci , hsd , cs, pa, cval.getName() , access_spec_found , cval.isInherited(), cval.isVirtualInherited(), base_class_offset) ;
            fvis.TraverseVarDecl(static_cast<clang::VarDecl *>(d)) ;
            cval.addFieldDescription(fvis.get_field_data()) ;
        }
        break ;
        case clang::Decl::AccessSpec : {
            access_spec_found = true ;
        }
        break ;
        default :
        break ;
    }
    return true ;
}

bool CXXRecordVisitor::VisitDecl(clang::Decl *d) {
    if ( debug_level >=2 ) {
        std::cout << "\nCXXRecordVisitor Decl = " << d->getDeclKindName() << std::endl ;
    }
    return true ;
}

bool CXXRecordVisitor::VisitType(clang::Type *t) {
    if ( debug_level >=2 ) {
        std::cout << "\nCXXRecordVisitor Type = " << t->getTypeClassName() << std::endl ;
    }
    return true;
}

bool CXXRecordVisitor::VisitCXXRecordDecl( clang::CXXRecordDecl *rec ) {
    if ( debug_level >= 2 ) {
        rec->dump() ; std::cout << std::endl ;
    }
    if ( rec->getDefinition() == NULL ) {
        return true ;
    }

    // If this class needs a default constructor, then the complier will generate one and we can call it.
    if ( rec->needsImplicitDefaultConstructor() ) {
        cval.setHasDefaultConstructor(true) ;
    } else {
        // Test all constructors to see if any of those are the default and public
        clang::CXXRecordDecl::ctor_iterator cit ;
        for ( cit = rec->ctor_begin() ; cit != rec->ctor_end() ; cit++ ) {
            if ( (*cit)->isDefaultConstructor() and (*cit)->getAccess() == clang::AS_public ) {
                cval.setHasDefaultConstructor(true) ;
            }
        }
    }

    // Test to see there is no declared destructor or it is declared public
    clang::CXXDestructorDecl * dd  = rec->getDestructor() ;
    if ( dd == NULL or dd->getAccess() == clang::AS_public ) {
        cval.setHasPublicDestructor(true) ;
    }

    std::string file_name = getFileName(ci , rec->getSourceRange().getEnd(), hsd) ;
    char * rp = almostRealPath(file_name.c_str()) ;
    if ( rp == NULL ) {
        return false ;
    }
    if ( hsd.isPathInICGExclude(rp) ) {
        free(rp) ;
        return false ;
    }

    cval.setFileName(rp) ;
    free(rp) ;
    cval.setAbstract(rec->isAbstract()) ;
    cval.setName(rec->getNameAsString()) ;
    cval.setPOD(rec->isPOD()) ;

    clang::CXXRecordDecl::base_class_iterator bcii ;

    //std::cout << "parsing " << cval.getName() << std::endl ;
    //std::cout << "    [34mprocessing inheritance " <<  rec->getNumBases() << " " << rec->getNumVBases() << "[00m" << std::endl ;
    for ( bcii = rec->bases_begin() ; bcii != rec->bases_end() ; bcii++ ) {
        if ( !bcii->isVirtual() ) {
            const clang::Type * temp = bcii->getType().getTypePtr() ;
            //std::cout << "\n[33minherited Type = " << temp->getTypeClassName() << "[00m" << std::endl ;
            const clang::RecordType * rt = temp->getAs<clang::RecordType>() ;
            if ( rt != NULL ) {
                clang::RecordDecl * rd = rt->getDecl() ;
                //std::cout << "    [34m" << cval.getName() << " inherits from " << rd->getNameAsString() << "[00m" << std::endl ;
                //rd->dump() ; std::cout << std::endl ;
                if ( isInUserOrTrickCode(ci , rd->getSourceRange().getEnd(), hsd) ) {
                    const clang::ASTRecordLayout &record_layout = rec->getASTContext().getASTRecordLayout(rec);
                    unsigned int inherit_class_offset ;

                    inherit_class_offset = record_layout.getBaseClassOffset(llvm::cast<clang::CXXRecordDecl>(rd)).getQuantity() ;
                    //std::cout << "    [34minherit_class_offset = " << inherit_class_offset << "[00m" << std::endl ;
                    //std::cout << "    [34m" << getFileName(ci , rd->getSourceRange().getEnd(), hsd) << "[00m" << std::endl ;
                    CXXRecordVisitor inherit_cvis(ci , cs, hsd , pa, true, bcii->isVirtual(), false, inherit_class_offset) ;
                    inherit_cvis.TraverseCXXRecordDecl(static_cast<clang::CXXRecordDecl *>(rd)) ;
                    cval.addInheritedFieldDescriptions(inherit_cvis.get_class_data()->getFieldDescription()) ;
                    // clear the field list in the inherited class so they are not freed when inherit_cvis goes out of scope.
                    inherit_cvis.get_class_data()->clearFieldDescription() ;
                    // If we are inheriting from a template specialization, don't save the inherited class.  This list
                    // is maintained to call init_attr of the inherited classes.  A template specialization does not have
                    // these attributes.
                    if ( temp->getTypeClass() != clang::Type::TemplateSpecialization ) {
                        // We want to save the inherited class data, but it's going to go out of scope so we need to make
                        // a copy of it.
                        ClassValues * icv = new ClassValues(*(inherit_cvis.get_class_data())) ;
                        // The inherited classes of this inherited class are not required in the copy, and they are going out of scope
                        cval.saveInheritAncestry(icv) ;
                        icv->clearInheritedClass() ;

                        // Save the copy of the inherited class to the current class
                        cval.addInheritedClass(icv) ;
                    }
                }
            }
        }
    }
    //std::cout << "    [34mend inheritance[00m" << std::endl ;

    /* Virtual base classes are parsed once at the top. Each inherited class call to CXXRecordVisitor sets this flag to false. */
    if ( include_virtual_base ) {
        for ( bcii = rec->vbases_begin() ; bcii != rec->vbases_end() ; bcii++ ) {
            const clang::Type * temp = bcii->getType().getTypePtr() ;
            //std::cout << "\n[33minherited Type = " << temp->getTypeClassName() << "[00m" << std::endl ;
            const clang::RecordType * rt = temp->getAs<clang::RecordType>() ;
            if ( rt != NULL ) {
                clang::RecordDecl * rd = rt->getDecl() ;
                //std::cout << "    [34m" << cval.getName() << " virtually inherits from " << rd->getNameAsString() << "[00m" << std::endl ;
                //rd->dump() ; std::cout << std::endl ;
                if ( isInUserOrTrickCode(ci , rd->getSourceRange().getEnd(), hsd) ) {
                    const clang::ASTRecordLayout &record_layout = rec->getASTContext().getASTRecordLayout(rec);
                    unsigned int inherit_class_offset ;

                    // Virtual base classes have a fixed offset that cannot be accessed at runtime.  We have clang's calculation
                    // to where the base class will be here.  Hopefully it matches what other compilers calculate. :(
                    inherit_class_offset = record_layout.getVBaseClassOffset(llvm::cast<clang::CXXRecordDecl>(rd)).getQuantity() ;

                    //std::cout << "    [34minherit_class_offset = " << inherit_class_offset << "[00m" << std::endl ;
                    //std::cout << "    [34m" << getFileName(ci , rd->getSourceRange().getEnd(), hsd) << "[00m" << std::endl ;
                    CXXRecordVisitor inherit_cvis(ci , cs, hsd , pa, true, bcii->isVirtual(), false, inherit_class_offset) ;
                    inherit_cvis.TraverseCXXRecordDecl(static_cast<clang::CXXRecordDecl *>(rd)) ;
                    cval.addInheritedFieldDescriptions(inherit_cvis.get_class_data()->getFieldDescription()) ;
                    // clear the field list in the inherited class so they are not freed when inherit_cvis goes out of scope.
                    inherit_cvis.get_class_data()->clearFieldDescription() ;
                    // If we are inheriting from a template specialization, don't save the inherited class.  This list
                    // is maintained to call init_attr of the inherited classes.  A template specialization does not have
                    // these attributes.
                    if ( temp->getTypeClass() != clang::Type::TemplateSpecialization ) {
                        ClassValues * icv = new ClassValues(*(inherit_cvis.get_class_data())) ;
                        // The inherited classes of this inherted class are not required in the copy, and they are going out of scope
                        icv->clearInheritedClass() ;

                        // Save the copy of the inherited class to the current class
                        cval.addInheritedClass(icv) ;
                    }
                }
            }
        }
    }

    cval.getNamespacesAndClasses(rec->getDeclContext()) ;

    // clears obscured inherited variables caused by diamond inheritance
    cval.clearAmbiguousVariables() ;

    // sets the container class for all fields to the current class name
    cval.setContainerClassForFields() ;

    return true;
}

bool CXXRecordVisitor::VisitFriendDecl( clang::FriendDecl *fd ) {
    //std::cout << "    [33mClassVisitor::VisitFriendDecl[00m" << std::endl ;
    clang::TypeSourceInfo * tsi = fd->getFriendType() ;
    if ( tsi == NULL ) {
        clang::NamedDecl * nd = fd->getFriendDecl() ;
        if ( nd != NULL and nd->getDeclName().isIdentifier()) {
            //nd->dump() ;
            std::string init_func = std::string("init_attr") ;
            std::stringstream name_sp ;
            cval.print_namespaces( name_sp , "__") ;
            init_func += name_sp.str() + cval.getName() ;
            std::string friend_str = nd->getName().str() ;
            if ( ! friend_str.find(init_func) ) {
                //std::cout << "        [32mfound a friend![00m" << std::endl ;
                cval.setHasInitAttrFriend(true) ;
            }
        }
    }
    return true ;
}

ClassValues * CXXRecordVisitor::get_class_data() {
    return &cval ;
}

