# trick-gxplot — ported from this directory's own makefile (the authority).
# parse_format.c was missing from the stale 2019 source list (Part A4: the
# stale files are hints, not the authority) — without it gp_view.cpp's
# parse_format() calls fail to link.
#
# HDF5_LIB / -lc++abi / DPC listed twice mirror the makefile's own ALL_LIBS +
# trailing ${CONTROLLER_LIBS} -lDPV (a real circular DPC<->DPV dependency the
# make link line works around by repeating -lDPC after -lDPV).
#
# -ltrick_units resolves against data_products/units's own libtrick_units.a,
# not trick_utils/units' same-named archive — see the comment in
# ../../Apps/Trk2csv/CMakeLists.txt for the -L-search-order evidence. Link
# dp_units, not trick_units.
set(GXPLOT_SRC
    parse_format.c
    gp_view.cpp
    gp_colors.cpp
    gp_utilities.cpp
    gp_version.cpp
    gxplot.cpp
    gp_view_curve_node.cpp
    gp_view_plot_node.cpp
    gp_view_page_node.cpp
    gp_view_product_node.cpp
)

add_executable(trick-gxplot ${GXPLOT_SRC})
target_include_directories(trick-gxplot PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
    ${CMAKE_SOURCE_DIR}/include
    ${X11_X11_INCLUDE_PATH}
    ${LIBXML2_INCLUDE_DIR}
)
separate_arguments(_tr_gxplot_udunits_includes NATIVE_COMMAND "${UDUNITS_INCLUDES}")
target_compile_options(trick-gxplot PRIVATE ${_tr_gxplot_udunits_includes})

target_link_libraries(trick-gxplot PRIVATE
    DPC DPM dp_log dp_var dp_eqparse dp_units
    dpv_utils
    ${X11_Xt_LIB} ${X11_X11_LIB}
    ${LIBXML2_LIBRARIES}
    DPC dpv_utils
    dl
)
separate_arguments(_tr_gxplot_udunits_ldflags NATIVE_COMMAND "${UDUNITS_LDFLAGS}")
target_link_options(trick-gxplot PRIVATE ${_tr_gxplot_udunits_ldflags})
if(HDF5_HOME)
    target_link_libraries(trick-gxplot PRIVATE -L${HDF5_HOME}/lib -lhdf5_hl -lhdf5 -lsz)
endif()
if(APPLE)
    target_link_libraries(trick-gxplot PRIVATE -lc++abi)
else()
    target_link_libraries(trick-gxplot PRIVATE rt)
endif()

install(TARGETS trick-gxplot RUNTIME DESTINATION bin)
