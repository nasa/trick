
###############################################################
# Build Java
###############################################################

add_custom_target(java COMMAND mvn package -Dcmake=true -DbuildDirectory=${CMAKE_BINARY_DIR}/libexec/trick/java/build WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
