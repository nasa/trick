
#ifndef FIELDVISITOR_HH
#define FIELDVISITOR_HH

#include <map>

#include "clang/Frontend/CompilerInstance.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "PrintAttributes.hh"

class CommentSaver ;
class HeaderSearchDirs ;
class SourceManager ;
class FieldDescription ;

/**

  This class is a recursive AST visitor that is called when a Field node inside a class
  is found.  We are in what type the field is, what the array dimensions are, and the
  bitfield lengths.  All information is stored in a FieldDescription object.

  @author Alexander S. Lin

  @date July 2012

 */

class FieldVisitor : public clang::RecursiveASTVisitor<FieldVisitor> {
    public:
        FieldVisitor(clang::CompilerInstance & in_ci ,
         HeaderSearchDirs & in_hsd ,
         CommentSaver & cs ,
         PrintAttributes & in_pa ,
         std::string container_class ) ;

        /* VisitDecl and VisitType are here for debug printing. */
        bool VisitDecl(clang::Decl *d) ;
        bool VisitType(clang::Type *t) ;

        /* These routines are called when nodes of the corresponding types are traversed */
        bool VisitBuiltinType(clang::BuiltinType *bt);
        bool VisitConstantArrayType(clang::ConstantArrayType *a) ;
        bool VisitDeclaratorDecl( clang::DeclaratorDecl *dd ) ;
        //bool VisitElaboratedType( clang::ElaboratedType *et ) ;
        bool VisitEnumType( clang::EnumType *et ) ;
        bool VisitFieldDecl( clang::FieldDecl *field ) ;
        bool VisitPointerType(clang::PointerType *p);
        bool VisitRecordType(clang::RecordType *rt);
        //bool VisitSubstTemplateTypeParmType(clang::SubstTemplateTypeParmType *sttpt);
        //bool VisitTemplateSpecializationType(clang::TemplateSpecializationType *tst);
        //bool VisitTemplateTypeParmType(clang::TemplateTypeParmType *ttp);
        //bool VisitTypedefType(clang::TypedefType *tt);
        bool VisitVarDecl( clang::VarDecl *v ) ;

        /** common code to process a template */
        bool ProcessTemplate( std::string in_name , clang::CXXRecordDecl * crd ) ;

        /** Returns the field data */
        FieldDescription * get_field_data() ;

    private:
        /** The compiler's source manager.  Holds file/line info for everything. */
        clang::CompilerInstance & ci ;

        /** Holds all comments */
        CommentSaver & cs ;

        /** The header search directories */
        HeaderSearchDirs & hsd ;

        /** attributes printer */
        PrintAttributes & pa ;

        /** Holds the field information found, usually returned to caller of this visitor. */
        FieldDescription * fdes ;

        /** Map of template name to mangled name of all templates visited */
        static std::map < std::string , std::string > processed_templates ;

} ;

#endif
