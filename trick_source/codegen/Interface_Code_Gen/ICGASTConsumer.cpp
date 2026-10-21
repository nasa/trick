
#include <iostream>
#include "clang/AST/ASTContext.h"
#include "ICGASTConsumer.hh"
#include "ICGDiagnosticConsumer.hh"

ICGASTConsumer::ICGASTConsumer( clang::CompilerInstance & in_ci , HeaderSearchDirs & in_hsd ,
 CommentSaver & in_cs , PrintAttributes & in_pa , const ICGDiagnosticConsumer & in_diagnostics ) :
 ci(in_ci) , hsd(in_hsd) , diagnostics(in_diagnostics) , tuv(in_ci, in_hsd, in_cs, in_pa) {}

TranslationUnitVisitor & ICGASTConsumer::getTranslationUnitVisitor() {
    return tuv ;
}

/**
@details
-# Traverse the translation unit declaration and everything it contains.
*/
void ICGASTConsumer::HandleTranslationUnit(clang::ASTContext &Ctx) {
    // Invalid user code cannot safely provide record layouts. Keep ignoring
    // system diagnostics that ICG's diagnostic consumer suppresses.
    if (diagnostics.error_in_user_code) {
        return;
    }
    tuv.TraverseDecl(Ctx.getTranslationUnitDecl());
}

