# libvar.a — ported from this directory's own makefile (the authority).
add_library(dp_var STATIC var.cpp)
target_include_directories(dp_var PRIVATE ${CMAKE_SOURCE_DIR}/include)
set_target_properties(dp_var PROPERTIES ARCHIVE_OUTPUT_NAME var)
