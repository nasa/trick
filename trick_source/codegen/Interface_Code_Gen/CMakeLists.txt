
set ( ICG_SRC
  ClassTemplateVisitor
  ClassValues
  ClassVisitor
  CommentSaver
  ConstructValues
  EnumValues
  EnumVisitor
  FieldDescription
  FieldVisitor
  FindTrickICG
  HeaderSearchDirs
  ICGASTConsumer
  PrintAttributes
  PrintFileContents10
  PrintFileContentsBase
  TranslationUnitVisitor
  TypedefVisitor
  Utilities
  VariableVisitor
  main
  ../../sim_services/UdUnits/map_trick_units_to_udunits
)

add_executable( trick-ICG ${ICG_SRC} )
target_compile_options( trick-ICG PUBLIC -g -DTRICK_VERSION="${TRICK_MAJOR}.${TRICK_MINOR}.${TRICK_TINY}" -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS)
target_compile_options( trick-ICG PUBLIC -DEXTERNAL_BUILD)
target_compile_options( trick-ICG PUBLIC -DLLVM_HOME="${LLVM_ROOT_DIR}")
target_compile_options( trick-ICG PUBLIC -DLIBCLANG_MAJOR=${LLVM_VERSION_MAJOR} -DLIBCLANG_MINOR=${LLVM_VERSION_MINOR} -DLIBCLANG_PATCHLEVEL=${LLVM_VERSION_PATCH})
target_include_directories( trick-ICG PUBLIC ${UDUNITS2_INCLUDES} )
target_include_directories( trick-ICG PUBLIC ${LLVM_INCLUDE_DIRS} )
set_property(SOURCE trick-ICG APPEND PROPERTY OBJECT_DEPENDS ${CMAKE_BINARY_DIR}/include/mongoose/mongoose.h)
set_target_properties( trick-ICG PROPERTIES CXX_STANDARD 14)

target_link_libraries( trick-ICG
    -lclangFrontend
    -lclangDriver
    -lclangSerialization
    -lclangParse
    -lclangSema
    -lclangAnalysis
    -lclangEdit
    -lclangAST
    -lclangLex
    -lclangBasic
    ${LLVM_LDFLAGS}
    ${LLVM_LIBRARIES}
    ${UDUNITS2_LIBRARIES}
)

