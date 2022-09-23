
#ifndef VARIABLEVISITOR_HH
#define VARIABLEVISITOR_HH

#include <string>
#include <set>

#include "clang/Frontend/CompilerInstance.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "PrintAttributes.hh"

class CommentSaver ;
class ClassValues ;
class EnumValues ;
class HeaderSearchDirs ;

/**

  This class is a recursive AST visitor that is called when a VariableDecl node
  is found.  We are interested in typedefs of templates. i.e.
  tyedef MyTemplateType< short, int, double > my_tt ; and of structs/classes.
  This class parses the underlying type and stores the struct/class
  a as a separate class from the origin type.

  A typedef visitor is usually instantiated by the top level visitor, TranslationUnitVisitor.

  @author Alexander S. Lin

  @date July 2012

 */

class VariableVisitor : public clang::RecursiveASTVisitor<VariableVisitor> {
    public:
        VariableVisitor( clang::CompilerInstance & in_ci , CommentSaver & cs ,
         HeaderSearchDirs & hsd , PrintAttributes & in_pa , std::set< std::string > & fdc) ;

        /* VisitDecl and VisitType are here for debug printing. */
        bool VisitDecl(clang::Decl *d) ;
        bool VisitType(clang::Type *t) ;

        /* These routines are called when nodes of the corresponding types are traversed */
/*
        bool VisitConstantArrayType(clang::ConstantArrayType *ct) ;
        bool VisitElaboratedType(clang::ElaboratedType *et) ;
        bool VisitPointerType(clang::PointerType *pt) ;
        bool VisitRecordType(clang::RecordType *rt) ;
        bool VisitTypedefDecl(clang::TypedefDecl *td) ;
*/
        bool VisitTemplateSpecializationType(clang::TemplateSpecializationType *tst) ;

    private:
        /** The compiler's source manager.  Holds file/line info for everything. */
        clang::CompilerInstance & ci ;

        /** The header search directories */
        HeaderSearchDirs & hsd ;

        /** Holds all comments */
        CommentSaver & cs ;

        /** Attributes printer */
        PrintAttributes & pa ;

        /** name of the typedef */
        std::string typedef_name ;

        /** does this typedef include array or pointer dimensions? */
        bool has_dims ;

        /** name of the typedef */
        clang::SourceRange typedef_location ;

        /** The DeclContext of the typedef declaration.  Used to determine namespaces of typedef */
        clang::DeclContext * typedef_decl_context ;

        /** Holds the class information found, usually returned to caller of this visitor. */
        ClassValues * cval ;

        /** Set of fwd declares in the current file this typedef resides in */
        std::set< std::string > & fwd_declared_classes ;

} ;

#endif
