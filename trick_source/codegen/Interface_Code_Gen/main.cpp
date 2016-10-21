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

// Command line arguments. These work better as globals, as suggested in llvm/CommandLine documentation.
llvm::cl::list<std::string> include_dirs("I", llvm::cl::Prefix, llvm::cl::desc("Include directory"), llvm::cl::value_desc("directory"));
llvm::cl::list<std::string> defines("D", llvm::cl::Prefix, llvm::cl::desc("Defines"), llvm::cl::value_desc("define"));
llvm::cl::opt<bool> units_truth_is_scary("units-truth-is-scary", llvm::cl::desc("Don't print units conversion messages"));
llvm::cl::opt<bool> sim_services_flag("s", llvm::cl::desc("Gernerate io_src for Trick core headers"));
llvm::cl::opt<bool> force("f", llvm::cl::desc("Force all io_src files to be generated"));
llvm::cl::opt<int> attr_version("v", llvm::cl::desc("Select version of attributes to produce. 10 and 13 are valid"), llvm::cl::init(10));
llvm::cl::opt<int> debug_level("d", llvm::cl::desc("Set debug level"), llvm::cl::init(0), llvm::cl::ZeroOrMore);
llvm::cl::opt<bool> create_map("m", llvm::cl::desc("Create map files"), llvm::cl::init(false));
llvm::cl::opt<std::string> output_dir("o", llvm::cl::desc("Output directory"));
llvm::cl::alias ssf_alias("sim_services" , llvm::cl::desc("Alias for -s") , llvm::cl::aliasopt(sim_services_flag));
llvm::cl::alias force_alias("force" , llvm::cl::desc("Alias for -f") , llvm::cl::aliasopt(force));
llvm::cl::list<std::string> input_file_names(llvm::cl::Positional, llvm::cl::desc("<input_file>"), llvm::cl::ZeroOrMore);
llvm::cl::list<std::string> sink(llvm::cl::Sink, llvm::cl::ZeroOrMore);
llvm::cl::list<std::string> pre_compiled_headers("include", llvm::cl::Prefix, llvm::cl::desc("pre-compiled headers"), llvm::cl::value_desc("pre_compiled_headers"));
llvm::cl::opt<bool> no_offset_of("n", llvm::cl::desc("Do not print the offsetof calculations in attributes"));
llvm::cl::alias no_offset_of_alias("no-offset-of" , llvm::cl::desc("Alias for -n") , llvm::cl::aliasopt(no_offset_of));

/**
Most of the main program is pieced together from examples on the web. We are doing the following:

-# Creating lists of include directories, defines, and input files from the command line arguments.
-# Initializing the compiler to read C++ code, and setting the compiler to think we are creating code for the default target architecture.
-# Creating the necessary source and file managers as well as the preprocessor.
-# Adding search directories and creating #define statements for -D command line arguments.
-# Telling clang to use our ICGASTConsumer as an ASTConsumer.
-# Parsing the input file.
*/
int main(int argc, char * argv[]) {
    llvm::cl::SetVersionPrinter([] {
        std::cout << "Trick Interface Code Generator (trick-ICG) " << TRICK_VERSION << std::endl;
    });

    /**
     * Gather all of the command line arguments into lists of include directories, defines, and input files.
     * All other arguments will be ignored.
     */
    llvm::cl::ParseCommandLineOptions(argc, argv);

    if (!validAttributesVersion(attr_version)) {
        return -1;
    }

    if (input_file_names.empty()) {
        std::cerr << "No header file specified" << std::endl;
        return 1;
    }

    clang::CompilerInstance compilerInstance;
    compilerInstance.createDiagnostics();
    compilerInstance.getDiagnosticOpts().ShowColors = 1;
    compilerInstance.getDiagnostics().setIgnoreAllWarnings(true);

    // Set all of the defaults to c++
    clang::CompilerInvocation::setLangDefaults(compilerInstance.getLangOpts(), clang::IK_CXX);
    compilerInstance.getLangOpts().CXXExceptions = true;

    // Activate C++11 parsing
    compilerInstance.getLangOpts().CPlusPlus11 = true;

    // Set the default target architecture
#if (LIBCLANG_MAJOR > 3) || ((LIBCLANG_MAJOR == 3) && (LIBCLANG_MINOR >= 5))
    clang::TargetOptions targetOptions;
    targetOptions.Triple = llvm::sys::getDefaultTargetTriple();
    compilerInstance.setTarget(clang::TargetInfo::CreateTargetInfo(compilerInstance.getDiagnostics(), std::make_shared<clang::TargetOptions>(targetOptions)));
#else
    clang::TargetOptions* targetOptions = new clang::TargetOptions();
    targetOptions->Triple = llvm::sys::getDefaultTargetTriple();
    compilerInstance.setTarget(clang::TargetInfo::CreateTargetInfo(compilerInstance.getDiagnostics(), targetOptions));
#endif

    // Create all of the necessary managers
    compilerInstance.createFileManager();
    compilerInstance.createSourceManager(compilerInstance.getFileManager());

    // Create the preprocessor
#if (LIBCLANG_MAJOR > 3) || ((LIBCLANG_MAJOR == 3) && (LIBCLANG_MINOR >= 5))
    compilerInstance.createPreprocessor(clang::TU_Complete);
#else
    compilerInstance.createPreprocessor();
#endif
    clang::Preprocessor& preprocessor = compilerInstance.getPreprocessor();

    // Add all of the include directories to the preprocessor
    HeaderSearchDirs headerSearchDirs(compilerInstance.getPreprocessor().getHeaderSearchInfo(), compilerInstance.getHeaderSearchOpts(), preprocessor, sim_services_flag);
    headerSearchDirs.addSearchDirs(include_dirs);

    // Tell the preprocessor to use its default predefines
    compilerInstance.getPreprocessorOpts().UsePredefines = true;
#if (LIBCLANG_MAJOR > 3) || ((LIBCLANG_MAJOR == 3) && (LIBCLANG_MINOR >= 8))
    preprocessor.getBuiltinInfo().initializeBuiltins(preprocessor.getIdentifierTable(), preprocessor.getLangOpts());
#else
    preprocessor.getBuiltinInfo().InitializeBuiltins(preprocessor.getIdentifierTable(), preprocessor.getLangOpts());
#endif
    // Add all of the #define from the command line to the default predefines
    headerSearchDirs.addDefines(defines);

    // Add our comment saver as a comment handler in the preprocessor
    CommentSaver commentSaver(compilerInstance, headerSearchDirs);
    preprocessor.addCommentHandler(&commentSaver);

    PrintAttributes printAttributes(attr_version, headerSearchDirs, commentSaver, compilerInstance, force, sim_services_flag, output_dir);

    // Create new class and enum map files
    if (create_map) {
        printAttributes.createMapFiles();
    }

    // Tell the compiler to use our ICGASTconsumer
    ICGASTConsumer* astConsumer = new ICGASTConsumer(compilerInstance, headerSearchDirs, commentSaver, printAttributes);
#if (LIBCLANG_MAJOR > 3) || ((LIBCLANG_MAJOR == 3) && (LIBCLANG_MINOR >= 6))
    compilerInstance.setASTConsumer(std::move(std::unique_ptr<clang::ASTConsumer>(astConsumer)));
#else
    compilerInstance.setASTConsumer(astConsumer);
#endif
    compilerInstance.createASTContext();
    compilerInstance.createSema(clang::TU_Prefix, NULL);

    // Get the full path of the file to be read
    char buffer[input_file_names[0].size()];
    strcpy(buffer, input_file_names[0].c_str());
    std::string path(dirname(buffer));
    path += "/";
    strcpy(buffer, input_file_names[0].c_str());
    path += basename(buffer);
    char* inputFilePath = almostRealPath(path);

    struct stat dummy;
    if (stat(inputFilePath, &dummy)) {
        std::cerr << "Could not open file " << inputFilePath << std::endl;
        exit(-1);
    }
    // Open up the input file and parse it
    const clang::FileEntry* fileEntry = compilerInstance.getFileManager().getFile(inputFilePath);
    free(inputFilePath);
#if (LIBCLANG_MAJOR > 3) || ((LIBCLANG_MAJOR == 3) && (LIBCLANG_MINOR >= 5))
    compilerInstance.getSourceManager().setMainFileID(compilerInstance.getSourceManager().createFileID(fileEntry, clang::SourceLocation(), clang::SrcMgr::C_User));
#else
    compilerInstance.getSourceManager().createMainFileID(fileEntry);
#endif
    compilerInstance.getDiagnosticClient().BeginSourceFile(compilerInstance.getLangOpts(), &compilerInstance.getPreprocessor());
    clang::ParseAST(compilerInstance.getSema());
    compilerInstance.getDiagnosticClient().EndSourceFile();

    if (!sim_services_flag) {
        printAttributes.printIOMakefile();
    }

    // Close the map files
    printAttributes.closeMapFiles();

    // Print the list of headers that have the ICG:(No) comment
    printAttributes.printICGNoFiles();

    return 0;
}
