
set( DPM_SRC
  DPM_attribute
  DPM_axis
  DPM_column
  DPM_component
  DPM_curve
  DPM_extfn
  DPM_inputs
  DPM_measurement
  DPM_outputs
  DPM_page
  DPM_parse_tree
  DPM_product
  DPM_relation
  DPM_run
  DPM_session
  DPM_table
  DPM_time_constraints
  DPM_var
)

add_library( DPM STATIC ${DPM_SRC})
target_include_directories( DPM PUBLIC ${LIBXML2_INCLUDE_DIR} )
target_include_directories( DPM PUBLIC ${UDUNITS2_INCLUDES} )
target_include_directories( DPM PUBLIC .. )

