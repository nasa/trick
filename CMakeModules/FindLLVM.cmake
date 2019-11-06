# Find the native LLVM includes and library
#
# LLVM_INCLUDE_DIR - where to find llvm include files
# LLVM_LIBRARY_DIR - where to find llvm libs
# LLVM_CFLAGS      - llvm compiler flags
# LLVM_LDFLAGS      - llvm linker flags
# LLVM_MODULE_LIBS - list of llvm libs for working with modules.
# LLVM_FOUND       - True if llvm found.
if (LLVM_INSTALL_DIR)
  find_program(LLVM_CONFIG_EXECUTABLE
               NAMES llvm-config-37 llvm-config-3.7 llvm-config-36 llvm-config-3.6 llvm-config-38 llvm-config-3.8 llvm-config-39 llvm-config-3.9 llvm-config llvm-config-35 llvm-config-3.5 llvm-config-34 llvm-config-3.4
               DOC "llvm-config executable"
               PATHS ${LLVM_INSTALL_DIR} NO_DEFAULT_PATH)
else (LLVM_INSTALL_DIR)
  find_program(LLVM_CONFIG_EXECUTABLE
               NAMES llvm-config-37 llvm-config-3.7 llvm-config-36 llvm-config-3.6 llvm-config-38 llvm-config-3.8 llvm-config-39 llvm-config-3.9 llvm-config llvm-config-35 llvm-config-3.5 llvm-config-34 llvm-config-3.4
               DOC "llvm-config executable")
endif (LLVM_INSTALL_DIR)

if (LLVM_CONFIG_EXECUTABLE)
  message(STATUS "LLVM llvm-config found at: ${LLVM_CONFIG_EXECUTABLE}")
else (LLVM_CONFIG_EXECUTABLE)
  message(FATAL_ERROR "Could NOT find LLVM executable, please add -DLLVM_INSTALL_DIR=/path/to/llvm-config/ in cmake command")
endif (LLVM_CONFIG_EXECUTABLE)
execute_process(
  COMMAND ${LLVM_CONFIG_EXECUTABLE} --version
  OUTPUT_VARIABLE LLVM_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
string(REGEX REPLACE "([0-9])\\.([0-9]*).*" "\\1\\2" LLVM_VERSION_NODOT ${LLVM_VERSION})
string(REGEX REPLACE "([0-9])\\.([0-9]*).*" "\\1.\\2" LLVM_VERSION_NOPATCH ${LLVM_VERSION})

if (LLVM_FIND_VERSION_MAJOR AND LLVM_FIND_VERSION_MINOR)
  SET(LLVM_FIND_VERSION_NODOT "${LLVM_FIND_VERSION_MAJOR}${LLVM_FIND_VERSION_MINOR}")
  if (LLVM_VERSION_NODOT VERSION_LESS LLVM_FIND_VERSION_NODOT)
    message(FATAL_ERROR "imcompatible LLVM version ${LLVM_VERSION} required ${LLVM_FIND_VERSION}")
  else (LLVM_VERSION_NODOT VERSION_LESS LLVM_FIND_VERSION_NODOT)
    if (LLVM_VERSION_NODOT VERSION_EQUAL LLVM_FIND_VERSION_NODOT)
      message(STATUS "find stable LLVM version ${LLVM_VERSION}")
    else (LLVM_VERSION_NODOT VERSION_EQUAL LLVM_FIND_VERSION_NODOT)
      message(STATUS "find unstable LLVM version ${LLVM_VERSION}")
    endif (LLVM_VERSION_NODOT VERSION_EQUAL LLVM_FIND_VERSION_NODOT)
    add_definitions("-DLLVM_${LLVM_VERSION_NODOT}")
  endif (LLVM_VERSION_NODOT VERSION_LESS LLVM_FIND_VERSION_NODOT)
endif (LLVM_FIND_VERSION_MAJOR AND LLVM_FIND_VERSION_MINOR)

if (LLVM_INSTALL_DIR)
  find_program(CLANG_EXECUTABLE
               NAMES clang-${LLVM_VERSION_NODOT} clang-${LLVM_VERSION_NOPATCH} clang
               PATHS ${LLVM_INSTALL_DIR} NO_DEFAULT_PATH)
  find_program(LLVM_AS_EXECUTABLE
               NAMES llvm-as-${LLVM_VERSION_NODOT} llvm-as-${LLVM_VERSION_NOPATCH} llvm-as
               PATHS ${LLVM_INSTALL_DIR} NO_DEFAULT_PATH)
  find_program(LLVM_LINK_EXECUTABLE
               NAMES llvm-link-${LLVM_VERSION_NODOT} llvm-link-${LLVM_VERSION_NOPATCH} llvm-link
               PATHS ${LLVM_INSTALL_DIR} NO_DEFAULT_PATH)
else (LLVM_INSTALL_DIR)
  find_program(CLANG_EXECUTABLE
               NAMES clang-${LLVM_VERSION_NODOT} clang-${LLVM_VERSION_NOPATCH} clang)
  find_program(LLVM_AS_EXECUTABLE
               NAMES llvm-as-${LLVM_VERSION_NODOT} llvm-as-${LLVM_VERSION_NOPATCH} llvm-as)
  find_program(LLVM_LINK_EXECUTABLE
               NAMES llvm-link-${LLVM_VERSION_NODOT} llvm-link-${LLVM_VERSION_NOPATCH} llvm-link)
endif (LLVM_INSTALL_DIR)

execute_process(
  COMMAND ${LLVM_CONFIG_EXECUTABLE} --includedir
  OUTPUT_VARIABLE LLVM_INCLUDE_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
  COMMAND ${LLVM_CONFIG_EXECUTABLE} --libdir
  OUTPUT_VARIABLE LLVM_LIBRARY_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
  COMMAND ${LLVM_CONFIG_EXECUTABLE} --cppflags
  OUTPUT_VARIABLE LLVM_CFLAGS
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
  COMMAND ${LLVM_CONFIG_EXECUTABLE} --ldflags
  OUTPUT_VARIABLE LLVM_LDFLAGS
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
  COMMAND ${LLVM_CONFIG_EXECUTABLE} --libs
  OUTPUT_VARIABLE LLVM_MODULE_LIBS
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (LLVM_VERSION_NODOT VERSION_GREATER 34)
execute_process(
  COMMAND ${LLVM_CONFIG_EXECUTABLE} --system-libs
  OUTPUT_VARIABLE LLVM_SYSTEM_LIBS_ORIG
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
if (LLVM_SYSTEM_LIBS_ORIG)
string(REGEX REPLACE " *\n" "" LLVM_SYSTEM_LIBS ${LLVM_SYSTEM_LIBS_ORIG})
endif (LLVM_SYSTEM_LIBS_ORIG)
endif (LLVM_VERSION_NODOT VERSION_GREATER 34)

macro(add_one_lib name)
  FIND_LIBRARY(CLANG_LIB
    NAMES ${name}
    PATHS ${LLVM_LIBRARY_DIR} NO_DEFAULT_PATH)
  set(CLANG_LIBRARIES ${CLANG_LIBRARIES} ${CLANG_LIB})
	unset(CLANG_LIB CACHE)
endmacro()

#Assume clang lib path same as llvm lib path
add_one_lib("clangCodeGen")
add_one_lib("clangFrontend")
add_one_lib("clangSerialization")
add_one_lib("clangDriver")
add_one_lib("clangSema")
add_one_lib("clangStaticAnalyzerFrontend")
add_one_lib("clangStaticAnalyzerCheckers")
add_one_lib("clangStaticAnalyzerCore")
add_one_lib("clangAnalysis")
add_one_lib("clangEdit")
add_one_lib("clangAST")
add_one_lib("clangParse")
add_one_lib("clangSema")
add_one_lib("clangLex")
add_one_lib("clangBasic")
