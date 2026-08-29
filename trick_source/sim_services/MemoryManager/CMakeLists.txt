set( TRICK_MM_SRC
  ADefParseContext.cpp
  AttributesUtils.cpp
  MemoryManager.cpp
  MemoryManager_C_Intf.cpp
  MemoryManager_JSON_Intf.cpp
  MemoryManager_add_attr_info.cpp
  MemoryManager_add_checkpoint_alloc_dependency.cpp
  MemoryManager_add_shared_library_symbols.cpp
  MemoryManager_add_template_name_trans.cpp
  MemoryManager_add_var.cpp
  MemoryManager_alloc_depends.cpp
  MemoryManager_alloc_info_map.cpp
  MemoryManager_clear_memory.cpp
  MemoryManager_declare_var.cpp
  MemoryManager_delete_var.cpp
  MemoryManager_get_attributes_for_address.cpp
  MemoryManager_get_enumerated.cpp
  MemoryManager_get_size.cpp
  MemoryManager_get_stl_dependencies.cpp
  MemoryManager_get_type_attributes.cpp
  MemoryManager_io_src_intf.cpp
  MemoryManager_is_alloced.cpp
  MemoryManager_make_declaration.cpp
  MemoryManager_make_reference_attr.cpp
  MemoryManager_map_external_object.cpp
  MemoryManager_realloc.cpp
  MemoryManager_ref_allocate.cpp
  MemoryManager_ref_assignment.cpp
  MemoryManager_ref_attributes.cpp
  MemoryManager_ref_dim.cpp
  MemoryManager_ref_name.cpp
  MemoryManager_ref_name_from_address.cpp
  MemoryManager_ref_var.cpp
  MemoryManager_restore.cpp
  MemoryManager_restore_stls.cpp
  MemoryManager_set_checkpointagent.cpp
  MemoryManager_set_debug_level.cpp
  MemoryManager_strdup.cpp
  MemoryManager_write_checkpoint.cpp
  MemoryManager_write_var.cpp
  RefParseContext.cpp
  addr_bitfield.c
  extract_bitfield.c
  extract_unsigned_bitfield.c
  follow_address_path.c
  insert_bitfield.c
  ref_free.cpp
  ref_to_value.c
  trickTypeCharString.c
  vval.c
  wcs_ext.c
)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

flex_target(trick_mm_adef_lexer adef_parser.l ${CMAKE_CURRENT_BINARY_DIR}/adef_parser.lex.cpp)
bison_target(trick_mm_adef_parser adef_parser.y ${CMAKE_CURRENT_BINARY_DIR}/adef_parser.tab.cpp)
flex_target(trick_mm_ref_lexer ref_parser.l ${CMAKE_CURRENT_BINARY_DIR}/ref_parser.lex.cpp)
bison_target(trick_mm_ref_parser ref_parser.y ${CMAKE_CURRENT_BINARY_DIR}/ref_parser.tab.cpp)

add_library( trick_mm STATIC
  ${TRICK_MM_SRC}
  ${FLEX_trick_mm_adef_lexer_OUTPUTS}
  ${BISON_trick_mm_adef_parser_OUTPUTS}
  ${FLEX_trick_mm_ref_lexer_OUTPUTS}
  ${BISON_trick_mm_ref_parser_OUTPUTS}
)
target_link_libraries( trick_mm PUBLIC trick_build_flags )
set_target_properties( trick_mm PROPERTIES ARCHIVE_OUTPUT_NAME trick_mm)

# Match the Makefile's per-generator-output warning suppressions
# (CheckPointAgent/Makefile applies the same pattern; see A3).
set_source_files_properties(
    ${FLEX_trick_mm_adef_lexer_OUTPUTS} ${FLEX_trick_mm_ref_lexer_OUTPUTS}
    PROPERTIES COMPILE_OPTIONS "-Wno-unused-parameter;-Wno-unused-function;-Wno-sign-compare;-x;c++"
)
set_source_files_properties(
    ${BISON_trick_mm_adef_parser_OUTPUTS} ${BISON_trick_mm_ref_parser_OUTPUTS}
    PROPERTIES COMPILE_OPTIONS "-Wno-unused-parameter;-Wno-unused;-x;c++"
)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set_property(SOURCE ${BISON_trick_mm_adef_parser_OUTPUTS} ${BISON_trick_mm_ref_parser_OUTPUTS}
        APPEND PROPERTY COMPILE_OPTIONS "-Wno-parentheses-equality")
endif()
