set( TRICK_VAR_BINARY_PARSER_SRC
  src/var_binary_parser.cc
)

add_library( trick_var_binary_parser STATIC ${TRICK_VAR_BINARY_PARSER_SRC})
target_include_directories( trick_var_binary_parser PRIVATE ${CMAKE_SOURCE_DIR}/include )
target_link_libraries( trick_var_binary_parser PUBLIC trick_build_flags )
set_target_properties( trick_var_binary_parser PROPERTIES ARCHIVE_OUTPUT_NAME trick_var_binary_parser)
