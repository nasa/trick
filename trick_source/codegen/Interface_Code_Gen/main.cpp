#include <iostream>
#include <sstream>
#include <libgen.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "llvm/Support/Host.h"
#include "llvm/Support/CommandLine.h"

#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/FileManager.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Parse/ParseAST.h"

#include "ICGASTConsumer.hh"
#include "HeaderSearchDirs.hh"
#include "CommentSaver.hh"
#include "TranslationUnitVisitor.hh"
#include "PrintAttributes.hh"
#include "PrintAttributesFactory.hh"
#include "Utilities.hh"

#include "trick/units_conv.h"

/* Command line arguments.  These work better as globals, as suggested in llvm/CommandLine documentation */
llvm::cl::list< std::string > include_dirs("I", llvm::cl::Prefix, llvm::cl::desc("Include directory"),
 llvm::cl::value_desc("directory")) ;
llvm::cl::list< std::string > defines("D", llvm::cl::Prefix, llvm::cl::desc("Defines"),
 llvm::cl::value_desc("define")) ;
llvm::cl::opt< bool > sim_services_flag ("s", llvm::cl::desc("Gernerate io_src for Trick core headers")) ;
llvm::cl::opt< bool > force ("f", llvm::cl::desc("Force all io_src files to be generated")) ;
llvm::cl::opt< int > attr_version ("v", llvm::cl::desc("Select version of attributes to produce.  10 and 13 are valid"), llvm::cl::init(10)) ;
llvm::cl::opt< int > debug_level ("d", llvm::cl::desc("Set debug level"), llvm::cl::init(0), llvm::cl::ZeroOrMore) ;
llvm::cl::alias ssf_alias ("sim_services" , llvm::cl::desc("Alias for -s") , llvm::cl::aliasopt(sim_services_flag)) ;
llvm::cl::alias force_alias ("force" , llvm::cl::desc("Alias for -f") , llvm::cl::aliasopt(force)) ;
llvm::cl::list< std::string > input_file_names(llvm::cl::Positional, llvm::cl::desc("<input_file>"), llvm::cl::ZeroOrMore) ;
llvm::cl::list< std::string > sink(llvm::cl::Sink, llvm::cl::ZeroOrMore) ;
llvm::cl::list< std::string > pre_compiled_headers("include", llvm::cl::Prefix, llvm::cl::desc("pre-compiled headers"),
 llvm::cl::value_desc("pre_compiled_headers")) ;
llvm::cl::opt< bool > show_units ("u", llvm::cl::desc("List recognized units")) ;

void ICG_version() {
    std::cout << "Trick Interface Code Generator (ICG) 13.dev" << std::endl ;
}

/**

Most of the main program is pieced together from examples on the web. We are doing the following.

-# Creating lists of include directories, defines, and input files from the command line arguments.
-# Initializing the compiler to read C++ code, and setting the compiler to think we are creating
   code for the default target architecture.
-# Creating the necessary source and file managers as well as the preprocessor.
-# Adding search directories and creating #define statements for -D command line arguments.
-# Telling clang to use our ICGASTConsumer as an ASTConsumer.
-# Parse the input file.

*/
int main( int argc , char * argv[] ) {

#if (__clang_major__ == 3) && (__clang_minor__ >= 5)
    clang::TargetOptions to;
#elif (__clang_major__ == 3) && (__clang_minor__ >= 3)
    clang::TargetOptions * to = new clang::TargetOptions() ;
#else
    clang::TargetOptions to;
#endif
    clang::CompilerInstance ci;

    /* Gather all of the command line arguments into lists of include directories, defines, and input files.
       All other arguments will be ignored. */
    llvm::cl::SetVersionPrinter(ICG_version) ;
    llvm::cl::ParseCommandLineOptions(argc , argv) ;

    if ( ! validAttributesVersion(attr_version) ) {
        return -1 ;
    }

    if ( show_units ) {
        list_units() ;
        return 0 ;
    }

#if (__clang_major__ == 3) && (__clang_minor__ >= 3)
    ci.createDiagnostics();
#else
    ci.createDiagnostics(0,NULL);
#endif
    clang::DiagnosticOptions & diago = ci.getDiagnosticOpts() ;
    diago.ShowColors = 1 ;
    ci.getDiagnostics().setIgnoreAllWarnings(true) ;
#if ( GCC_MAJOR == 4 ) && ( GCC_MINOR <= 2 )
    ci.getDiagnostics().setSuppressAllDiagnostics() ;
#endif

    // Set all of the defaults to c++
    clang::CompilerInvocation::setLangDefaults(ci.getLangOpts() , clang::IK_CXX) ;
    ci.getLangOpts().CXXExceptions = true ;

    // Activate C++11 parsing
#if (__clang_major__ == 3) && (__clang_minor__ >= 3)
    ci.getLangOpts().CPlusPlus11 = true ;
#else
    ci.getLangOpts().CPlusPlus0x = true ;
#endif

    // Set the default target architecture
#if (__clang_major__ == 3) && (__clang_minor__ >= 5)
    to.Triple = llvm::sys::getDefaultTargetTriple();
#elif (__clang_major__ == 3) && (__clang_minor__ >= 3)
    to->Triple = llvm::sys::getDefaultTargetTriple();
#else
    to.Triple = llvm::sys::getDefaultTargetTriple();
#endif

#if (__clang_major__ == 3) && (__clang_minor__ >= 5)
    std::shared_ptr<clang::TargetOptions> shared_to  = std::make_shared<clang::TargetOptions>(to) ;
    clang::TargetInfo *pti = clang::TargetInfo::CreateTargetInfo(ci.getDiagnostics(), shared_to);
#else
    clang::TargetInfo *pti = clang::TargetInfo::CreateTargetInfo(ci.getDiagnostics(), to);
#endif
    ci.setTarget(pti);

    // Create all of the necessary managers.
    ci.createFileManager();
    ci.createSourceManager(ci.getFileManager());
#if (__clang_major__ == 3) && (__clang_minor__ >= 5)
    ci.createPreprocessor(clang::TU_Complete);
#else
    ci.createPreprocessor();
#endif

    clang::HeaderSearch & hs = ci.getPreprocessor().getHeaderSearchInfo() ;
    clang::HeaderSearchOptions & hso = ci.getHeaderSearchOpts() ;
    clang::Preprocessor & pp = ci.getPreprocessor() ;

    // Add all of the include directories to the preprocessor
    HeaderSearchDirs hsd(hs , hso , pp, sim_services_flag) ;
    hsd.addSearchDirs ( include_dirs ) ;

    // Tell the preprocessor to use its default predefines
    clang::PreprocessorOptions & ppo = ci.getPreprocessorOpts() ;
    ppo.UsePredefines = true;
    pp.getBuiltinInfo().InitializeBuiltins(pp.getIdentifierTable(), pp.getLangOpts());
    // Add all of the #define from the command line to the default predefines.
    hsd.addDefines ( defines ) ;

    // Add our comment saver as a comment handler in the preprocessor
    CommentSaver cs(ci.getSourceManager()) ;
#if (__clang_major__ == 3) && (__clang_minor__ >= 3)
    pp.addCommentHandler(&cs) ;
#else
    pp.AddCommentHandler(&cs) ;
#endif


    PrintAttributes pa( attr_version, hsd, cs, ci, force, sim_services_flag) ;

    // create new class and enum map files
    pa.createMapFiles() ;

    // Tell the compiler to use our ICGASTconsumer
    ICGASTConsumer *astConsumer = new ICGASTConsumer(ci, hsd, cs, pa);
#if (__clang_major__ == 3) && (__clang_minor__ >= 6)
    std::unique_ptr<clang::ASTConsumer> unique_ast(astConsumer) ;
    ci.setASTConsumer(std::move(unique_ast));
#else
    ci.setASTConsumer(astConsumer);
#endif
    ci.createASTContext();
    ci.createSema(clang::TU_Prefix, NULL);

    // Get the full path of the file to be read.
    char * input_file_cp = strdup(input_file_names[0].c_str()) ;
    char * input_file_file = basename(input_file_cp) ;
    char * input_file_dir = dirname(input_file_cp) ;
    char * input_file_full_path = NULL ;
    std::stringstream os ;
    os << input_file_dir << "/" << input_file_file ;
    input_file_full_path = almostRealPath( os.str().c_str() ) ;
    //std::cout << input_file_full_path << std::endl ;

    // Open up the input file and parse it.
    const clang::FileEntry *pFile = ci.getFileManager().getFile(input_file_full_path);
#if (__clang_major__ == 3) && (__clang_minor__ >= 5)
    ci.getSourceManager().setMainFileID(ci.getSourceManager().createFileID(pFile, clang::SourceLocation(), clang::SrcMgr::C_User));
#else
    ci.getSourceManager().createMainFileID(pFile);
#endif
    ci.getDiagnosticClient().BeginSourceFile(ci.getLangOpts(), &ci.getPreprocessor());
    clang::ParseAST(ci.getSema());
    ci.getDiagnosticClient().EndSourceFile();

    free(input_file_cp) ;
    free(input_file_full_path) ;

    // Print empty io_src files for headers that we visited, but have no class/struct information.
    pa.printEmptyFiles() ;

    // Close the map files
    pa.closeMapFiles() ;

    // Print the list of headers that have the ICG:(No) comment
    pa.printICGNoFiles() ;

    return 0;
}
