#include "clang/Basic/Builtins.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/PreprocessorOptions.h"
#include "clang/Parse/ParseAST.h"
#include "llvm/Support/CommandLine.h"
// `llvm/Support/Host.h` is deprecated in favour of `llvm/TargetParser/Host.h` since clang 17
#if LIBCLANG_MAJOR > 16
#include "llvm/TargetParser/Host.h"
#else
#include "llvm/Support/Host.h"
#endif
#include "llvm/Support/raw_ostream.h"

#include <string>

// This TU is link-tested (not run) by cmake/TrickClangLibs.cmake to verify the
// selected C++ compiler can actually link the LLVM/Clang archives trick-ICG
// needs. main() must therefore *reference a std::string-returning LLVM symbol*,
// not just return: an empty main() links even when the compiler's C++ standard
// library (libc++ vs libstdc++) disagrees with the one LLVM was built against,
// because it pulls no library symbols — which lets a mismatched compiler slip
// past configure and only fail when trick-ICG itself is linked.
//
// llvm::sys::getDefaultTargetTriple() returns std::string and exists in every
// supported LLVM version, so the compiler emits a reference to *its* standard
// library's mangling of it (e.g. the libstdc++ [abi:cxx11] symbol under gcc vs
// the libc++ std::__1 symbol under a libc++ clang). That reference only
// resolves when the archives were built with the same standard library, so a
// mismatch turns into a link failure here — on any platform, without hardcoding
// per-OS/per-compiler assumptions.
int main()
{
    std::string triple = llvm::sys::getDefaultTargetTriple();
    return triple.empty() ? 1 : 0;
}
