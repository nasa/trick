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

# Link order matters here: GNU ld (default on Linux) resolves symbols in a
# single left-to-right pass, so a library must appear *after* whatever
# consumes its symbols. This mirrors the makefile's ALL_LIBS + final link
# line order exactly (CONTROLLER_LIBS MODEL_LIBS DP_LIBS HDF5_LIB -ldl
# UDUNITS_LDFLAGS XLIBS LIBRTDEF CONTROLLER_LIBS -lDPV) — a prior version of
# this file used a separate target_link_options() call for UDUNITS_LDFLAGS,
# which does not guarantee it lands after the DPC/DPM/dp_units archives that
# actually need it, and silently linked fine on macOS (ld64's linker is far
# more forgiving of ordering) while failing with "undefined reference to
# `ut_parse'" etc. on Linux.
separate_arguments(_tr_gxplot_udunits_ldflags NATIVE_COMMAND "${UDUNITS_LDFLAGS}")
set(_tr_gxplot_libs
    DPC
    DPM
    dp_log dp_var dp_eqparse dp_units
)
if(HDF5_HOME)
    list(APPEND _tr_gxplot_libs -L${HDF5_HOME}/lib -lhdf5_hl -lhdf5 -lsz)
endif()
list(APPEND _tr_gxplot_libs
    dl
    ${_tr_gxplot_udunits_ldflags}
    ${X11_Xt_LIB} ${X11_X11_LIB}
    ${LIBXML2_LIBRARIES}
)
if(APPLE)
    list(APPEND _tr_gxplot_libs -lc++abi)
else()
    list(APPEND _tr_gxplot_libs rt)
endif()
list(APPEND _tr_gxplot_libs DPC dpv_utils)
target_link_libraries(trick-gxplot PRIVATE ${_tr_gxplot_libs})

install(TARGETS trick-gxplot RUNTIME DESTINATION bin)
