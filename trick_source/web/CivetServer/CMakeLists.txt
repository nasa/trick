# libtrickCivet.a — Trick's native civetweb variable-server webserver,
# ported from this directory's own makefile (the authority). Only added to
# the build when USE_CIVETWEB is set (Phase 4, gated on TRICK_CIVETWEB_HOME
# detection in cmake/TrickPrograms.cmake).
#
# trick/MyCivetServer.hh (and this whole feature) is compiled out entirely
# unless USE_CIVETWEB is defined, mirroring Makefile.common:162-167's
# TRICK_SYSTEM_CXXFLAGS += -DUSE_CIVETWEB — trick_build_flags already adds
# that definition globally when USE_CIVETWEB is on (root CMakeLists.txt).

add_library(trickCivet STATIC
    src/MyCivetServer.cpp
    src/VariableServerSession.cpp
    src/VariableServerVariable.cpp
    src/http_GET_handlers.cpp
    src/simpleJSON.cpp
)
target_include_directories(trickCivet PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CIVETWEB_HOME}/include
)
target_link_libraries(trickCivet PUBLIC trick_build_flags)
set_target_properties(trickCivet PROPERTIES ARCHIVE_OUTPUT_NAME trickCivet)
