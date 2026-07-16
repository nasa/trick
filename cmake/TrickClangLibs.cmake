# Implements the ICG_CLANGLIBS selection algorithm from
# autoconf/configure.ac:222-256 exactly, then sanity-checks the result with a
# try_compile against TestICGLinkedLibs.cpp.
#
# Requires LLVM_LIB_DIR and CLANG_VERSION (from FindLLVMClang.cmake).
# Exports: ICG_CLANGLIBS, TR_LLVM_LIBS, TR_LLVM_SYSTEM_LIBS, TR_LLVM_LDFLAGS
# (llvm-config --libs/--system-libs/--ldflags, the latter pre-filtered per
# platform — see below). trick_source/codegen/Interface_Code_Gen/CMakeLists.txt
# links trick-ICG from these instead of re-running llvm-config itself.

if(NOT DEFINED LLVM_LIB_DIR)
    message(FATAL_ERROR "TrickClangLibs.cmake requires LLVM_LIB_DIR (include FindLLVMClang first)")
endif()

if(CLANG_VERSION AND CLANG_VERSION VERSION_GREATER_EQUAL "18.0.0")
    set(_tr_old_clang_libs
        "-lclangFrontend -lclangDriver -lclangSerialization -lclangParse -lclangSema -lclangAnalysis -lclangEdit -lclangAST -lclangASTMatchers -lclangAPINotes -lclangLex -lclangBasic"
    )
else()
    set(_tr_old_clang_libs
        "-lclangFrontend -lclangDriver -lclangSerialization -lclangParse -lclangSema -lclangAnalysis -lclangEdit -lclangAST -lclangLex -lclangBasic"
    )
endif()
set(_tr_new_clang_libs "-lclang-cpp")

if(EXISTS "${LLVM_LIB_DIR}/libclangFrontend.a" OR EXISTS "${LLVM_LIB_DIR}/libclangFrontend.so")
    set(ICG_CLANGLIBS "${_tr_old_clang_libs}")
elseif(EXISTS "${LLVM_LIB_DIR}/libclang-cpp.a" OR EXISTS "${LLVM_LIB_DIR}/libclang-cpp.so")
    set(ICG_CLANGLIBS "${_tr_new_clang_libs}")
else()
    message(FATAL_ERROR "could not find clang libs in LLVM library: \"${LLVM_LIB_DIR}\"")
endif()

if(EXISTS "${LLVM_LIB_DIR}/libclangSupport.a")
    set(ICG_CLANGLIBS "${ICG_CLANGLIBS} -lclangSupport")
endif()
if(EXISTS "${LLVM_LIB_DIR}/libclangOptions.a")
    set(ICG_CLANGLIBS "${ICG_CLANGLIBS} -lclangOptions")
endif()
if(EXISTS "${LLVM_LIB_DIR}/libclangAnalysisLifetimeSafety.a")
    set(ICG_CLANGLIBS "${ICG_CLANGLIBS} -lclangAnalysisLifetimeSafety")
endif()

# Sanity-*link* a real clang/LLVM TU against the derived ICG_CLANGLIBS. This
# is the whole point of TestICGLinkedLibs.cpp: catch a wrong library
# selection at configure time rather than at Phase 2's trick-ICG link step.
# The link recipe mirrors trick_source/codegen/Interface_Code_Gen/Makefile
# exactly (CLANGLIBS/LLVMLDFLAGS/CXXFLAGS construction).
string(REGEX MATCH "^[0-9]+" _tr_llvm_version_major "${LLVM_VERSION_STRING}")

execute_process(COMMAND ${LLVM_CONFIG_EXECUTABLE} --libs
    OUTPUT_VARIABLE TR_LLVM_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${LLVM_CONFIG_EXECUTABLE} --system-libs
    OUTPUT_VARIABLE _tr_llvm_system_libs_raw OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${LLVM_CONFIG_EXECUTABLE} --ldflags
    OUTPUT_VARIABLE TR_LLVM_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

# TR_LLVM_SYSTEM_LIBS: --system-libs filtered per platform, computed once
# here and reused by both this file's sanity try_compile below and
# Interface_Code_Gen/CMakeLists.txt's trick-ICG link.
set(TR_LLVM_SYSTEM_LIBS "")
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(LLVM_VERSION_STRING VERSION_GREATER_EQUAL "3.5")
        # Fedora adds -ledit as a system lib, but it isn't installed or required.
        string(REPLACE "-ledit" "" TR_LLVM_SYSTEM_LIBS "${_tr_llvm_system_libs_raw}")
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(TR_LLVM_SYSTEM_LIBS "${_tr_llvm_system_libs_raw}")
endif()

set(_tr_icg_link_libs "${ICG_CLANGLIBS} ${TR_LLVM_LIBS} ${TR_LLVM_SYSTEM_LIBS}")
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    if(_tr_llvm_version_major GREATER_EQUAL 16)
        set(_tr_icg_link_libs "${_tr_icg_link_libs} -lc++abi -lclang-cpp")
    else()
        set(_tr_icg_link_libs "${_tr_icg_link_libs} -lc++abi")
    endif()
endif()

if(_tr_llvm_version_major GREATER_EQUAL 16)
    set(_tr_icg_cxx_std 17)
elseif(_tr_llvm_version_major GREATER_EQUAL 10)
    set(_tr_icg_cxx_std 14)
else()
    set(_tr_icg_cxx_std 11)
endif()

separate_arguments(_tr_icg_link_libs_list NATIVE_COMMAND "-L${LLVM_LIB_DIR} ${TR_LLVM_LDFLAGS} ${_tr_icg_link_libs}")

try_compile(TR_ICG_CLANG_HEADERS_COMPILE
    ${CMAKE_BINARY_DIR}/CMakeFiles/TrickClangLibsCheck
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/TestICGLinkedLibs.cpp
    CMAKE_FLAGS
        "-DINCLUDE_DIRECTORIES=${LLVM_INCLUDE_DIR}"
    COMPILE_DEFINITIONS
        -DLIBCLANG_MAJOR=${_tr_llvm_version_major}
    LINK_LIBRARIES
        ${_tr_icg_link_libs_list}
    CXX_STANDARD ${_tr_icg_cxx_std}
    OUTPUT_VARIABLE TR_ICG_CLANG_HEADERS_COMPILE_OUTPUT
)
if(NOT TR_ICG_CLANG_HEADERS_COMPILE)
    message(FATAL_ERROR
"trick-ICG cannot be linked against the LLVM/Clang libraries in
  ${LLVM_LIB_DIR}
using
  ${CMAKE_CXX_COMPILER} (${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}).

The usual cause is a C++ standard-library (ABI) mismatch: the LLVM/Clang
archives are built against one standard library (libc++ or libstdc++) and the
selected C++ compiler defaults to the other, so the std::string / std::* symbols
the archives reference cannot be resolved. The compiler and LLVM must agree on
the standard library.
  * macOS: Homebrew/system LLVM is built against libc++. Build with a libc++
    Clang -- the default 'c++'/Apple Clang, or Homebrew clang. GNU gcc/g++ uses
    libstdc++ and has no working libc++ mode, so it cannot link these archives;
    unset CC/CXX or set them to clang/clang++.
  * Linux: distro LLVM is built against libstdc++; gcc and the system clang both
    default to libstdc++, so either works. Avoid clang -stdlib=libc++ unless your
    LLVM was also built that way.
Alternatively, point -DLLVM_HOME at an LLVM built with the same standard library
as your compiler.

ICG_CLANGLIBS was: \"${ICG_CLANGLIBS}\"
Sanity-link output:
${TR_ICG_CLANG_HEADERS_COMPILE_OUTPUT}")
endif()
