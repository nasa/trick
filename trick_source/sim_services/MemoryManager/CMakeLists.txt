set( TRICK_MM_SRC
  ADefParseContext
  MemoryManager
  MemoryManager_C_Intf
  MemoryManager_JSON_Intf
  MemoryManager_add_attr_info
  MemoryManager_add_checkpoint_alloc_dependency
  MemoryManager_add_shared_library_symbols
  MemoryManager_add_template_name_trans
  MemoryManager_add_var
  MemoryManager_alloc_depends
  MemoryManager_alloc_info_map
  MemoryManager_clear_memory
  MemoryManager_declare_var
  MemoryManager_delete_var
  MemoryManager_get_enumerated
  MemoryManager_get_size
  MemoryManager_get_stl_dependencies
  MemoryManager_get_type_attributes
  MemoryManager_io_src_intf
  MemoryManager_is_alloced
  MemoryManager_make_declaration
  MemoryManager_make_reference_attr
  MemoryManager_map_external_object
  MemoryManager_realloc
  MemoryManager_ref_allocate
  MemoryManager_ref_assignment
  MemoryManager_ref_attributes
  MemoryManager_ref_dim
  MemoryManager_ref_name
  MemoryManager_ref_name_from_address
  MemoryManager_ref_var
  MemoryManager_restore
  MemoryManager_restore_stls
  MemoryManager_set_checkpointagent
  MemoryManager_set_debug_level
  MemoryManager_strdup
  MemoryManager_write_checkpoint
  MemoryManager_write_var
  RefParseContext
  addr_bitfield
  extract_bitfield
  extract_unsigned_bitfield
  follow_address_path
  insert_bitfield
  parameter_types
  ref_free
  ref_to_value
  trickTypeCharString
  vval
  wcs_ext
)

# Sim services Lex/Yacc files
set( MM_LEX_YACC_SRC
  ${CMAKE_BINARY_DIR}/temp_src/lex_yacc/adef_parser.lex
  ${CMAKE_BINARY_DIR}/temp_src/lex_yacc/adef_parser.tab
  ${CMAKE_BINARY_DIR}/temp_src/lex_yacc/ref_parser.lex
  ${CMAKE_BINARY_DIR}/temp_src/lex_yacc/ref_parser.tab
)

add_custom_command( OUTPUT ${CMAKE_BINARY_DIR}/temp_src/lex_yacc/adef_parser.lex.cpp
 COMMAND ${FLEX_EXECUTABLE} -d -o ${CMAKE_BINARY_DIR}/temp_src/lex_yacc/adef_parser.lex.cpp ${CMAKE_CURRENT_SOURCE_DIR}/adef_parser.l
 MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/adef_parser.l
)
add_custom_command( OUTPUT ${CMAKE_BINARY_DIR}/temp_src/lex_yacc/adef_parser.tab.cpp ${CMAKE_BINARY_DIR}/temp_src/lex_yacc/adef_parser.tab.hpp
 COMMAND ${BISON_EXECUTABLE} -d -o ${CMAKE_BINARY_DIR}/temp_src/lex_yacc/adef_parser.tab.cpp ${CMAKE_CURRENT_SOURCE_DIR}/adef_parser.y
 MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/adef_parser.y
)
add_custom_command( OUTPUT ${CMAKE_BINARY_DIR}/temp_src/lex_yacc/ref_parser.lex.cpp
 COMMAND ${FLEX_EXECUTABLE} -d -o ${CMAKE_BINARY_DIR}/temp_src/lex_yacc/ref_parser.lex.cpp ${CMAKE_CURRENT_SOURCE_DIR}/ref_parser.l
 MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/ref_parser.l
)
add_custom_command( OUTPUT ${CMAKE_BINARY_DIR}/temp_src/lex_yacc/ref_parser.tab.cpp ${CMAKE_BINARY_DIR}/temp_src/lex_yacc/ref_parser.tab.hpp
 COMMAND ${BISON_EXECUTABLE} -d -o ${CMAKE_BINARY_DIR}/temp_src/lex_yacc/ref_parser.tab.cpp ${CMAKE_CURRENT_SOURCE_DIR}/ref_parser.y
 MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/ref_parser.y
)

add_library( trick_mm STATIC ${TRICK_MM_SRC} ${MM_LEX_YACC_SRC})
target_include_directories( trick_mm PUBLIC ${UDUNITS2_INCLUDES} )

