# libDPV.a — ported from this directory's own makefile (the authority).
add_library(dpv_utils STATIC DPV_textbuffer.cpp)
target_include_directories(dpv_utils PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set_target_properties(dpv_utils PROPERTIES ARCHIVE_OUTPUT_NAME DPV)
