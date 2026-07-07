# Trick's Java GUIs (trickview, simcontrol, dp, sie, ...), ported from this
# directory's own Makefile (top-level Makefile:242-274) and this project's
# pom.xml, which already has a "cmake" Maven profile
# (<activation><property><name>cmake</name><value>true</value>) that
# redirects the build output directory to -DbuildDirectory, matching the
# stale 2019 CMakeLists.txt's invocation — salvaged from there, but as a
# real add_custom_command with dependency tracking instead of a
# hand-maintained ~150-file source DEPENDS list, which had already drifted
# from the source tree once (see the sim_services/trick_utils source-list
# note in the Phase 2 status writeup).
#
# TRICK_OFFLINE copies prebuilt jars from trick-offline/ instead of invoking
# mvn (top-level Makefile:245-272); mirrored here rather than deferring to
# `mvn -o` since offline mode's jars are pre-built releases, not merely a
# local Maven cache.

set(TRICK_JAVA_BUILD_DIR ${CMAKE_BINARY_DIR}/libexec/trick/java/build)

if(NOT TRICK_OFFLINE)
    file(GLOB_RECURSE TRICK_JAVA_SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/java/*.java
    )

    add_custom_command(
        OUTPUT ${TRICK_JAVA_BUILD_DIR}/.stamp
        COMMAND ${MVN_EXECUTABLE} -q package -Dcmake=true
                -DbuildDirectory=${TRICK_JAVA_BUILD_DIR}
                -Dmaven.wagon.http.retryHandler.count=15
        COMMAND ${CMAKE_COMMAND} -E touch ${TRICK_JAVA_BUILD_DIR}/.stamp
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS ${TRICK_JAVA_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/pom.xml
        COMMENT "Building Trick Java GUIs (mvn package)"
        VERBATIM
    )
    add_custom_target(trick-java ALL DEPENDS ${TRICK_JAVA_BUILD_DIR}/.stamp)
else()
    set(TRICK_JAVA_OFFLINE_JARS
        DP Dre JXPlot MM MTV QP Sie SimControl SimSniffer TrickView
        trick-java-${TRICK_MAJOR}.${TRICK_MINOR}.${TRICK_TINY}
    )
    set(TRICK_JAVA_OFFLINE_TARGETS "")
    foreach(_tr_jar ${TRICK_JAVA_OFFLINE_JARS})
        set(_tr_src ${CMAKE_SOURCE_DIR}/trick-offline/${_tr_jar}.jar)
        set(_tr_dst ${TRICK_JAVA_BUILD_DIR}/${_tr_jar}.jar)
        add_custom_command(
            OUTPUT ${_tr_dst}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${TRICK_JAVA_BUILD_DIR}
            COMMAND ${CMAKE_COMMAND} -E copy ${_tr_src} ${_tr_dst}
            DEPENDS ${_tr_src}
            COMMENT "Copying offline jar ${_tr_jar}.jar"
            VERBATIM
        )
        list(APPEND TRICK_JAVA_OFFLINE_TARGETS ${_tr_dst})
    endforeach()
    add_custom_target(trick-java ALL DEPENDS ${TRICK_JAVA_OFFLINE_TARGETS})
endif()
