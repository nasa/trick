# liblog.a — ported from this directory's own makefile (the authority).
# TrickHDF5.cpp/DataStreamFactory.cpp's extra -DHDF5/-I only apply when HDF5
# was detected (makefile:24-28), mirrored below with per-file overrides.
set(DP_LOG_SRC
    Csv.cpp
    DataStream.cpp
    DataStreamFactory.cpp
    DataStreamGroup.cpp
    Delta.cpp
    ExternalProgram.cpp
    MatLab.cpp
    MatLab4.cpp
    TrickBinary.cpp
    log.cpp
    multiLog.cpp
    parseLogHeader.cpp
    trick_byteswap.cpp
)
if(HDF5_HOME)
    list(APPEND DP_LOG_SRC TrickHDF5.cpp)
endif()

add_library(dp_log STATIC ${DP_LOG_SRC})
target_include_directories(dp_log PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(dp_log PRIVATE Trick::udunits2)
set_target_properties(dp_log PROPERTIES ARCHIVE_OUTPUT_NAME log)

# Deliberately per-source rather than target_link_libraries(... trick::hdf5):
# source properties can't consume a target's usage requirements, and this
# scoping must match the Makefile's file-local override exactly (link-time
# HDF5 libs come from trick::hdf5 on the FXPLOT/GXPLOT executables that link
# dp_log).
if(HDF5_HOME)
    set_source_files_properties(TrickHDF5.cpp DataStreamFactory.cpp PROPERTIES
        COMPILE_OPTIONS "-I${HDF5_HOME}/include;-DHDF5"
    )
endif()
