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
#include "clang/Lex/PreprocessorOptions.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Parse/ParseAST.h"

#include "ICGASTConsumer.hh"
#include "HeaderSearchDirs.hh"
#include "CommentSaver.hh"
#include "TranslationUnitVisitor.hh"
#include "PrintAttributes.hh"
#include "PrintAttributesFactory.hh"
#include "Utilities.hh"

#include "units_conv.h"

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

    if ( input_file_names.empty() ) {
        std::cerr << "No header file specified" << std::endl ;
        return 1 ;
    }

    clang::CompilerInstance ci;
#if (LIBCLANG_MAJOR == 3) && (LIBCLANG_MINOR < 9)
    clang::CompilerInvocation::setLangDefaults(ci.getLangOpts() , clang::IK_CXX) ;
#endif
    ci.createDiagnostics();
    ci.getDiagnosticOpts().ShowColors = 1 ;
    ci.getDiagnostics().setIgnoreAllWarnings(true) ;
    ci.getLangOpts().CXXExceptions = true ;
    // Activate C++11 parsing
    ci.getLangOpts().Bool = true ;
    ci.getLangOpts().WChar = true ;
    ci.getLangOpts().CPlusPlus = true ;
    ci.getLangOpts().CPlusPlus11 = true ;
    ci.getLangOpts().CXXOperatorNames = true ;

    // Create all of the necessary managers.
    ci.createFileManager();
    ci.createSourceManager(ci.getFileManager());

    clang::PreprocessorOptions & ppo = ci.getPreprocessorOpts() ;
    ppo.UsePredefines = true;

#if (LIBCLANG_MAJOR > 3) || ((LIBCLANG_MAJOR == 3) && (LIBCLANG_MINOR >= 5))
    clang::TargetOptions to;
    to.Triple = llvm::sys::getDefaultTargetTriple();
    std::shared_ptr<clang::TargetOptions> shared_to  = std::make_shared<clang::TargetOptions>(to) ;
    clang::TargetInfo *pti = clang::TargetInfo::CreateTargetInfo(ci.getDiagnostics(), shared_to);
    ci.setTarget(pti);
    ci.createPreprocessor(clang::TU_Complete);
#else
    clang::TargetOptions * to = new clang::TargetOptions() ;
    to->Triple = llvm::sys::getDefaultTargetTriple();
    clang::TargetInfo *pti = clang::TargetInfo::CreateTargetInfo(ci.getDiagnostics(), to);
    ci.setTarget(pti);
    ci.createPreprocessor();
#endif

    // Set all of the defaults to c++
#if (LIBCLANG_MAJOR > 3) || ((LIBCLANG_MAJOR == 3) && (LIBCLANG_MINOR >= 9))
    llvm::Triple trip (to.Triple) ;
    clang::CompilerInvocation::setLangDefaults(ci.getLangOpts(), clang::IK_CXX, trip, ppo) ;
    // setting the language defaults clears the c++11 flag.
    ci.getLangOpts().CPlusPlus11 = true ;
#endif
    clang::Preprocessor& pp = ci.getPreprocessor();

    // Add all of the include directories to the preprocessor
    HeaderSearchDirs hsd(ci.getPreprocessor().getHeaderSearchInfo(), ci.getHeaderSearchOpts(), pp, sim_services_flag) ;
    hsd.addSearchDirs ( include_dirs ) ;

#if (LIBCLANG_MAJOR > 3) || ((LIBCLANG_MAJOR == 3) && (LIBCLANG_MINOR >= 8))
    pp.getBuiltinInfo().initializeBuiltins(pp.getIdentifierTable(), pp.getLangOpts());
#else
    pp.getBuiltinInfo().InitializeBuiltins(pp.getIdentifierTable(), pp.getLangOpts());
#endif
    // Add all of the #define from the command line to the default predefines.
    hsd.addDefines ( defines ) ;

    // Add our comment saver as a comment handler in the preprocessor
    CommentSaver cs(ci.getSourceManager()) ;
    pp.addCommentHandler(&cs) ;

    PrintAttributes pa( attr_version, hsd, cs, ci, force, sim_services_flag) ;

    // create new class and enum map files
    pa.createMapFiles() ;

    // Tell the compiler to use our ICGASTconsumer
    ICGASTConsumer *astConsumer = new ICGASTConsumer(ci, hsd, cs, pa);
#if (LIBCLANG_MAJOR > 3) || ((LIBCLANG_MAJOR == 3) && (LIBCLANG_MINOR >= 6))
    ci.setASTConsumer(std::move(std::unique_ptr<clang::ASTConsumer>(astConsumer)));
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
#if (LIBCLANG_MAJOR > 3) || ((LIBCLANG_MAJOR == 3) && (LIBCLANG_MINOR >= 5))
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
