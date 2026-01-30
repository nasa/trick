
#ifndef ENUMVISITOR_HH
#define ENUMVISITOR_HH

#include <map>
#include <string>
#include <vector>

#include "EnumValues.hh"
#include "clang/AST/RecursiveASTVisitor.h"

namespace clang
{
class CompilerInstance;
} // namespace clang
class HeaderSearchDirs;

/**

  This class is a recursive AST visitor that is called when a Enumeration node
  is found.  Inside of a Enum we are only interested in 2 types of nodes, the
  Enum itself, And the string enumerations contained within.

  @author Alexander S. Lin

  @date July 2012

 */

class EnumVisitor : public clang::RecursiveASTVisitor<EnumVisitor>
{
public:
    EnumVisitor(clang::CompilerInstance & in_ci, HeaderSearchDirs & in_hsd);

    /* VisitDecl and VisitType are here for debug printing. */
    bool VisitDecl(clang::Decl * d);
    bool VisitType(clang::Type * t);

    /* These routines are called when nodes of the corresponding types are traversed */
    bool VisitEnumDecl(clang::EnumDecl * ed);
    bool VisitEnumConstantDecl(clang::EnumConstantDecl * ecd);
    bool VisitEnumType(clang::EnumType * et);

    /** Returns the class data */
    EnumValues * get_enum_data();

private:
    /** The compiler instance. */
    clang::CompilerInstance & ci;

    /** The header search directories */
    HeaderSearchDirs & hsd;

    /** Holds the class information found, usually returned to caller of this visitor. */
    EnumValues eval;
};

#endif
