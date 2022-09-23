
# need to add TrickHDF5 if HDF5 found
set ( DP_LOG_SRC
  Csv
  DataStream
  DataStreamFactory
  DataStreamGroup
  Delta
  ExternalProgram
  MatLab
  MatLab4
  TrickBinary
  log
  multiLog
  parseLogHeader
  trick_byteswap
)
# TrickHDF5

add_library( dp_log STATIC ${DP_LOG_SRC})
target_include_directories( dp_log PUBLIC .. )
target_include_directories( dp_log PUBLIC ${UDUNITS2_INCLUDES} )


