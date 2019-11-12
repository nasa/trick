
set( DPC_SRC
  DPC_TimeCstrDataStream
  DPC_UnitConvDataStream
  DPC_datastream_supplier
  DPC_delta_curve
  DPC_delta_plot
  DPC_page
  DPC_plot
  DPC_product
  DPC_standard_plot
  DPC_std_curve
  DPC_table
)

add_library( DPC STATIC ${DPC_SRC})
target_include_directories( DPC PUBLIC ${LIBXML2_INCLUDE_DIR} )
target_include_directories( DPC PUBLIC ${UDUNITS2_INCLUDES} )
target_include_directories( DPC PUBLIC .. )

