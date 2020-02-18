
###############################################################
# Build Java
###############################################################

add_custom_target(java ALL COMMAND ${MAVEN_EXECUTABLE} -q package -Dcmake=true -DbuildDirectory=${CMAKE_BINARY_DIR}/libexec/trick/java/build -DTRICK_VERSION="${TRICK_MAJOR}.${TRICK_MINOR}.${TRICK_TINY}${TRICK_PRERELEASE}" WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
