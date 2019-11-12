
set( GXPLOT_SRC
  gp_colors
  gp_utilities
  gp_version
  gp_view
  gp_view_curve_node
  gp_view_page_node
  gp_view_plot_node
  gp_view_product_node
  gxplot
)

add_executable( trick-gxplot ${GXPLOT_SRC})
target_include_directories( trick-gxplot PUBLIC ${LIBXML2_INCLUDE_DIR} )
target_include_directories( trick-gxplot PUBLIC ../.. )
target_link_libraries( trick-gxplot DPC DPM dp_log dp_var dpv_utils dp_eqparse dp_units
 -ldl
 -L/usr/X11R6/lib64 -L/usr/X11R6/lib -lXt -lX11
 ${UDUNITS2_LIBRARIES} ${LIBXML2_LIBRARIES})

