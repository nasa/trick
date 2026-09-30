# trick-ICG: the Interface Code Generator. It parses headers with Clang and
# writes the io_src code that describes their types to Trick's memory manager.
add_executable(trick-ICG
    ClassTemplateVisitor.cpp
    ClassValues.cpp
    ClassVisitor.cpp
    CommentSaver.cpp
    ConstructValues.cpp
    EnumValues.cpp
    EnumVisitor.cpp
    FieldDescription.cpp
    FieldVisitor.cpp
    FindTrickICG.cpp
    HeaderSearchDirs.cpp
    ICGASTConsumer.cpp
    ICGDiagnosticConsumer.cpp
    main.cpp
    PrintAttributes.cpp
    PrintFileContents10.cpp
    PrintFileContentsBase.cpp
    TranslationUnitVisitor.cpp
    TypedefVisitor.cpp
    Utilities.cpp
    VariableVisitor.cpp
    ${PROJECT_SOURCE_DIR}/trick_source/sim_services/UdUnits/map_trick_units_to_udunits.cpp
)

target_compile_features(trick-ICG PRIVATE cxx_std_17)
target_include_directories(trick-ICG PRIVATE "${PROJECT_SOURCE_DIR}/include")
target_include_directories(trick-ICG SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})

# ICG's sources select Clang API differences by version, locate Clang's
# builtin headers under LLVM_HOME, and report Trick's version.
target_compile_definitions(trick-ICG PRIVATE
    __STDC_CONSTANT_MACROS
    __STDC_FORMAT_MACROS
    __STDC_LIMIT_MACROS
    LIBCLANG_MAJOR=${LLVM_VERSION_MAJOR}
    LIBCLANG_MINOR=${LLVM_VERSION_MINOR}
    LIBCLANG_PATCHLEVEL=${LLVM_VERSION_PATCH}
    LLVM_HOME="${LLVM_INSTALL_PREFIX}"
    TRICK_VERSION="${TRICK_VERSION_FULL}")
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # The GCC version whose extensions ICG tells Clang to accept.
    target_compile_definitions(trick-ICG PRIVATE
        TRICK_GCC_VERSION="${CMAKE_CXX_COMPILER_VERSION}")
endif()
# LLVM and Clang are built without RTTI.
target_compile_options(trick-ICG PRIVATE -fno-rtti)

# Clang's targets carry their dependencies, including LLVM's libraries, so only
# the components ICG uses directly are named. Some distributions install only
# the combined clang-cpp library.
if(TARGET clangFrontend)
    target_link_libraries(trick-ICG PRIVATE
        clangFrontend clangParse clangSema clangLex clangAST clangBasic)
else()
    target_link_libraries(trick-ICG PRIVATE clang-cpp)
endif()
target_link_libraries(trick-ICG PRIVATE UDUNITS2::UDUNITS2)

if(APPLE AND EXISTS "${LLVM_LIBRARY_DIR}/libLTO.dylib")
    # As the Make build does, give the system linker the libLTO from the LLVM
    # ICG links against, in case anything linked contains LLVM bitcode.
    target_link_options(trick-ICG PRIVATE
        "LINKER:-lto_library,${LLVM_LIBRARY_DIR}/libLTO.dylib")
endif()

# Keep LLVM's library directory on the installed executable's runtime path.
set_target_properties(trick-ICG PROPERTIES INSTALL_RPATH_USE_LINK_PATH ON)
