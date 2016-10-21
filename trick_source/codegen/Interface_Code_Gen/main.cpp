#include <iostream>
#include <sstream>
#include <libgen.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
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

/* Command line arguments.  These work better as globals, as suggested in llvm/CommandLine documentation */
llvm::cl::list< std::string > include_dirs("I", llvm::cl::Prefix, llvm::cl::desc("Include directory"),
 llvm::cl::value_desc("directory")) ;
llvm::cl::list< std::string > defines("D", llvm::cl::Prefix, llvm::cl::desc("Defines"),
 llvm::cl::value_desc("define")) ;
llvm::cl::opt< bool > units_truth_is_scary ("units-truth-is-scary", llvm::cl::desc("Don't print units conversion messages")) ;
llvm::cl::opt< bool > sim_services_flag ("s", llvm::cl::desc("Gernerate io_src for Trick core headers")) ;
llvm::cl::opt< bool > force ("f", llvm::cl::desc("Force all io_src files to be generated")) ;
llvm::cl::opt< int > attr_version ("v", llvm::cl::desc("Select version of attributes to produce.  10 and 13 are valid"), llvm::cl::init(10)) ;
llvm::cl::opt< int > debug_level ("d", llvm::cl::desc("Set debug level"), llvm::cl::init(0), llvm::cl::ZeroOrMore) ;
llvm::cl::opt< bool > create_map ("m", llvm::cl::desc("Create map files"), llvm::cl::init(false)) ;
llvm::cl::opt< std::string > output_dir ("o", llvm::cl::desc("Output directory")) ;
llvm::cl::alias ssf_alias ("sim_services" , llvm::cl::desc("Alias for -s") , llvm::cl::aliasopt(sim_services_flag)) ;
llvm::cl::alias force_alias ("force" , llvm::cl::desc("Alias for -f") , llvm::cl::aliasopt(force)) ;
llvm::cl::list< std::string > input_file_names(llvm::cl::Positional, llvm::cl::desc("<input_file>"), llvm::cl::ZeroOrMore) ;
llvm::cl::list< std::string > sink(llvm::cl::Sink, llvm::cl::ZeroOrMore) ;
llvm::cl::list< std::string > pre_compiled_headers("include", llvm::cl::Prefix, llvm::cl::desc("pre-compiled headers"),
 llvm::cl::value_desc("pre_compiled_headers")) ;

llvm::cl::opt< bool > global_compat15 ("c", llvm::cl::desc("Print the offsetof calculations in attributes")) ;
llvm::cl::alias compat15_alias ("compat15" , llvm::cl::desc("Alias for -c") , llvm::cl::aliasopt(global_compat15)) ;

void ICG_version() {
    std::cout << "Trick Interface Code Generator (trick-ICG) " << TRICK_VERSION << std::endl ;
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

#if (LIBCLANG_MAJOR > 3) || ((LIBCLANG_MAJOR == 3) && (LIBCLANG_MINOR >= 5))
    clang::TargetOptions to;
#else
    clang::TargetOptions * to = new clang::TargetOptions() ;
#endif
    clang::CompilerInstance ci;

    /* Gather all of the command line arguments into lists of include directories, defines, and input files.
       All other arguments will be ignored. */
    llvm::cl::SetVersionPrinter(ICG_version) ;
    llvm::cl::ParseCommandLineOptions(argc , argv) ;

    if ( ! validAttributesVersion(attr_version) ) {
        return -1 ;
    }

    if ( input_file_names.empty() ) {
        std::cerr << "No header file specified" << std::endl ;
        return 1 ;
    }

    ci.createDiagnostics();
    clang::DiagnosticOptions & diago = ci.getDiagnosticOpts() ;
    diago.ShowColors = 1 ;
    ci.getDiagnostics().setIgnoreAllWarnings(true) ;

    // Set all of the defaults to c++
    clang::CompilerInvocation::setLangDefaults(ci.getLangOpts() , clang::IK_CXX) ;
    ci.getLangOpts().CXXExceptions = true ;

    // Activate C++11 parsing
    ci.getLangOpts().CPlusPlus11 = true ;

    // Set the default target architecture
#if (LIBCLANG_MAJOR > 3) || ((LIBCLANG_MAJOR == 3) && (LIBCLANG_MINOR >= 5))
    to.Triple = llvm::sys::getDefaultTargetTriple();
#else
    to->Triple = llvm::sys::getDefaultTargetTriple();
#endif

#if (LIBCLANG_MAJOR > 3) || ((LIBCLANG_MAJOR == 3) && (LIBCLANG_MINOR >= 5))
    std::shared_ptr<clang::TargetOptions> shared_to  = std::make_shared<clang::TargetOptions>(to) ;
    clang::TargetInfo *pti = clang::TargetInfo::CreateTargetInfo(ci.getDiagnostics(), shared_to);
#else
    clang::TargetInfo *pti = clang::TargetInfo::CreateTargetInfo(ci.getDiagnostics(), to);
#endif
    ci.setTarget(pti);

    // Create all of the necessary managers.
    ci.createFileManager();
    ci.createSourceManager(ci.getFileManager());
#if (LIBCLANG_MAJOR > 3) || ((LIBCLANG_MAJOR == 3) && (LIBCLANG_MINOR >= 5))
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
#if (LIBCLANG_MAJOR > 3) || ((LIBCLANG_MAJOR == 3) && (LIBCLANG_MINOR >= 8))
    pp.getBuiltinInfo().initializeBuiltins(pp.getIdentifierTable(), pp.getLangOpts());
#else
    pp.getBuiltinInfo().InitializeBuiltins(pp.getIdentifierTable(), pp.getLangOpts());
#endif
    // Add all of the #define from the command line to the default predefines.
    hsd.addDefines ( defines ) ;

    // Add our comment saver as a comment handler in the preprocessor
    CommentSaver cs(ci, hsd) ;
    pp.addCommentHandler(&cs) ;

    PrintAttributes pa( attr_version, hsd, cs, ci, force, sim_services_flag, output_dir) ;

    // create new class and enum map files
    if ( create_map ) {
        pa.createMapFiles() ;
    }

    // Tell the compiler to use our ICGASTconsumer
    ICGASTConsumer *astConsumer = new ICGASTConsumer(ci, hsd, cs, pa);
#if (LIBCLANG_MAJOR > 3) || ((LIBCLANG_MAJOR == 3) && (LIBCLANG_MINOR >= 6))
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

    struct stat buffer ;
    if ( stat ( input_file_full_path , &buffer) != 0 ) {
        std::cerr << "Could not open file " << input_file_full_path << std::endl ;
        exit(-1) ;
    }
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

    if ( ! sim_services_flag ) {
        pa.printIOMakefile() ;
    }

    // Close the map files
    pa.closeMapFiles() ;

    // Print the list of headers that have the ICG:(No) comment
    pa.printICGNoFiles() ;

    return 0;
}
