
set( FXPLOT_SRC
  curve_view_node
  fermi_view
  fxplot
  page_view_node
  parse_format
  plot_view_node
  post_dialog
  product_view_node
  table_view_node
)

add_executable( trick-fxplot ${FXPLOT_SRC})
target_include_directories( trick-fxplot PUBLIC ${LIBXML2_INCLUDE_DIR} )
target_include_directories( trick-fxplot PUBLIC ../.. )
target_include_directories( trick-fxplot PUBLIC ../../../fermi-ware )
target_link_libraries( trick-fxplot DPC DPM dp_log dp_var dpv_utils fermiware dp_eqparse dp_units
 -ldl
 -L/usr/X11R6/lib64 -L/usr/X11R6/lib -lXt -lX11 -lXm
 ${UDUNITS2_LIBRARIES} ${LIBXML2_LIBRARIES})

