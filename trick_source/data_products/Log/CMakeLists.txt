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

if(HDF5_HOME)
    set_source_files_properties(TrickHDF5.cpp DataStreamFactory.cpp PROPERTIES
        COMPILE_OPTIONS "-I${HDF5_HOME}/include;-DHDF5"
    )
endif()
