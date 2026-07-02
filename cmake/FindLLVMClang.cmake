# Locate llvm-config and derive the LLVM/clang paths Trick's ICG needs.
# Mirrors autoconf/m4/tr_llvm_home.m4 and configure.ac:198-220.
#
# Honors the LLVM_HOME cache variable (equivalent to --with-llvm=DIR); when
# unset, searches the same fallback directories tr_llvm_home.m4 does, plus
# the Intel-mac homebrew prefix for parity with the old FindLLVM.cmake.
#
# Exports:
#   LLVM_CONFIG_EXECUTABLE, LLVM_HOME (prefix), LLVM_LIB_DIR, LLVM_BIN_DIR,
#   LLVM_INCLUDE_DIR, LLVM_VERSION_STRING, CLANG_EXECUTABLE, CLANG_VERSION

if(LLVM_HOME)
    find_program(LLVM_CONFIG_EXECUTABLE
        NAMES llvm-config
        PATHS "${LLVM_HOME}/bin"
        NO_DEFAULT_PATH
    )
    if(NOT LLVM_CONFIG_EXECUTABLE)
        message(FATAL_ERROR "could not find llvm-config")
    endif()
else()
    find_program(LLVM_CONFIG_EXECUTABLE
        NAMES llvm-config
        PATHS
            /usr/lib64/llvm20/bin
            /usr/lib64/llvm/bin
            /bin
            /usr/bin
            /usr/local/bin
            /sw/bin
            /opt/homebrew/opt/llvm/bin
            /usr/local/opt/llvm/bin
        NO_DEFAULT_PATH
    )
    if(NOT LLVM_CONFIG_EXECUTABLE)
        find_program(LLVM_CONFIG_EXECUTABLE NAMES llvm-config)
    endif()
    if(NOT LLVM_CONFIG_EXECUTABLE)
        message(FATAL_ERROR "could not find llvm-config")
    endif()
    execute_process(
        COMMAND ${LLVM_CONFIG_EXECUTABLE} --prefix
        OUTPUT_VARIABLE LLVM_HOME
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

execute_process(
    COMMAND ${LLVM_CONFIG_EXECUTABLE} --libdir
    OUTPUT_VARIABLE LLVM_LIB_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND ${LLVM_CONFIG_EXECUTABLE} --bindir
    OUTPUT_VARIABLE LLVM_BIN_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND ${LLVM_CONFIG_EXECUTABLE} --includedir
    OUTPUT_VARIABLE LLVM_INCLUDE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND ${LLVM_CONFIG_EXECUTABLE} --version
    OUTPUT_VARIABLE LLVM_VERSION_STRING
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT EXISTS "${LLVM_INCLUDE_DIR}/clang/Basic/LLVM.h")
    message(FATAL_ERROR "could not find clang development headers")
endif()
if(NOT EXISTS "${LLVM_LIB_DIR}/libLLVMSupport.a")
    message(FATAL_ERROR "could not find clang library files in ${LLVM_LIB_DIR}")
endif()

find_program(CLANG_EXECUTABLE
    NAMES clang
    PATHS "${LLVM_BIN_DIR}" /bin /usr/bin /usr/local/bin /sw/bin
    NO_DEFAULT_PATH
)
if(NOT CLANG_EXECUTABLE)
    message(FATAL_ERROR "could not find clang")
endif()

if(LLVM_VERSION_STRING VERSION_LESS "3.4.2")
    message(FATAL_ERROR "Trick requires llvm/clang version >= 3.4.2, found ${LLVM_VERSION_STRING}")
endif()

# autoconf/m4/tr_clang_version.m4 derives CLANG_VERSION from `clang --version`
# rather than trusting llvm-config, since some distros patch clang's reported
# version away from LLVM's. Replicate that here for parity.
execute_process(
    COMMAND ${CLANG_EXECUTABLE} --version
    OUTPUT_VARIABLE _tr_clang_version_output
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
string(REGEX MATCH "version ([0-9]+\\.[0-9]+\\.[0-9]+)" _ "${_tr_clang_version_output}")
set(CLANG_VERSION "${CMAKE_MATCH_1}")
if(CLANG_VERSION AND CLANG_VERSION VERSION_LESS "3.4.2")
    message(FATAL_ERROR "Trick requires llvm/clang version >= 3.4.2, found ${CLANG_VERSION}")
endif()
