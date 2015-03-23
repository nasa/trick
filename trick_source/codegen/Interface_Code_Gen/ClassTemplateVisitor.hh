
#ifndef CLASSTEMPLATEVISITOR_HH
#define CLASSTEMPLATEVISITOR_HH

#include <string>
#include <set>

#include "clang/Frontend/CompilerInstance.h"
#include "clang/AST/RecursiveASTVisitor.h"

class CommentSaver ;
class EnumValues ;
class HeaderSearchDirs ;
class PrintAttributes ;

/**

  This class is a recursive AST visitor that is called when a ClassTemplateDecl node
  is found.

  A class template visitor is usually instantiated by the top level visitor, TranslationUnitVisitor.

  @author Alexander S. Lin

  @date July 2012

 */

class ClassTemplateVisitor : public clang::RecursiveASTVisitor<ClassTemplateVisitor> {

    public:
        ClassTemplateVisitor( clang::CompilerInstance & in_ci , CommentSaver & cs ,
         HeaderSearchDirs & hsd , PrintAttributes & pa ) ;

        /* A custom traversal that handles only the node types we are interested in. */
        bool TraverseDecl(clang::Decl *D);

        /* VisitDecl and VisitType are here for debug printing. */
        bool VisitDecl(clang::Decl *d) ;
        bool VisitType(clang::Type *t) ;

        /* These routines are called when nodes of the corresponding types are traversed */

    private:
        /** The compiler's source manager.  Holds file/line info for everything. */
        clang::CompilerInstance & ci ;

        /** The header search directories */
        HeaderSearchDirs & hsd ;

        /** Holds all comments */
        CommentSaver & cs ;

        /** Attributes Printer */
        PrintAttributes & pa ;

} ;

#endif
