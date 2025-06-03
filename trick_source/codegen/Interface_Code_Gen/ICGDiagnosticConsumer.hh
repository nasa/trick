#ifndef ICG_DIAGNOSTICCONSUMER_HH
#define ICG_DIAGNOSTICCONSUMER_HH

#include "llvm/Support/raw_ostream.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"


class HeaderSearchDirs;

/**

  This class is passed to the clang parser as a DiagnosticConsumer. 
  It will terminate the trick build if an error found is from user code.

  @date May 2024
  
 */


class ICGDiagnosticConsumer : public clang::TextDiagnosticPrinter {
public:
    ICGDiagnosticConsumer(llvm::raw_ostream &os, clang::DiagnosticOptions *diags, clang::CompilerInstance &in_ci, HeaderSearchDirs &in_hsd);
    ~ICGDiagnosticConsumer() override;

    void HandleDiagnostic(clang::DiagnosticsEngine::Level DiagLevel,
                          const clang::Diagnostic &Info) override;

    /** Flag for if any error found in user code. */
    bool error_in_user_code;

protected:
    /** The compiler instance. */
    clang::CompilerInstance &ci ;

    /** The header search directories. */
    HeaderSearchDirs &hsd ;
    
};

#endif
