# trick-fxplot — the Motif/fermi-ware plotter, ported from this directory's own
# makefile (the authority; the 2019 stale CMakeLists.txt is untrusted per
# CMAKE_MIGRATION_PLAN.md A4). Sibling of GXPLOT (../GXPLOT/CMakeLists.txt) —
# same structure and the same GNU-ld single-pass link-order care.
#
# UNVERIFIED: fermi-ware ("fermi-ware/") is a NASA-internal tree absent from
# this checkout, so this whole target is gated (data_products/DPX/CMakeLists.txt
# only add_subdirectory's APPS/FXPLOT when fermi-ware/CMakeLists.txt exists) and
# has never been built here. The `fermi` link target below is the name the
# makefile implies (it links fermi-ware's `libfermi.a`); whoever restores
# fermi-ware must ensure its CMakeLists provides a target producing libfermi.a
# (ARCHIVE_OUTPUT_NAME fermi). Motif has no X11::Xm imported target (Motif is
# not core X11), so it stays on MOTIF_HOME, matching the makefile.
set(FXPLOT_SRC
    parse_format.c
    post_dialog.c
    fermi_view.cpp
    fxplot.cpp
    curve_view_node.cpp
    plot_view_node.cpp
    table_view_node.cpp
    page_view_node.cpp
    product_view_node.cpp
)

add_executable(trick-fxplot ${FXPLOT_SRC})
target_include_directories(trick-fxplot PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
    ${CMAKE_CURRENT_SOURCE_DIR}/../../..
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/trick_source/data_products/fermi-ware
)
if(MOTIF_HOME)
    target_include_directories(trick-fxplot PRIVATE ${MOTIF_HOME}/include)
endif()

# Link order matters (GNU ld resolves left-to-right in one pass, so a library
# must follow whatever consumes its symbols). Mirrors the makefile's final link
# line: OBJECT_FILES then ALL_LIBS (DPX_LIBS FERMI_WARE_LIB DP_LIBS
# TRICK_UNIT_LIBS LIBXML HDF5_LIB -ldl UDUNITS_LDFLAGS) then XLIBS (Motif -lXm
# -lXt -lX11) then LIBRTDEF (-lrt on Linux). See ../GXPLOT/CMakeLists.txt for
# the same reasoning and the macOS-vs-Linux linker-strictness note.
set(_tr_fxplot_libs
    DPC
    DPM
    fermi
    dp_log dp_var dp_eqparse dp_units
    LibXml2::LibXml2
)
if(HDF5_HOME)
    list(APPEND _tr_fxplot_libs trick::hdf5)
endif()
list(APPEND _tr_fxplot_libs
    dl
    Trick::udunits2
)
# Motif (-lXm) has no imported target; link it from MOTIF_HOME like the makefile.
if(MOTIF_HOME AND NOT MOTIF_HOME STREQUAL "/usr")
    list(APPEND _tr_fxplot_libs -L${MOTIF_HOME}/lib)
endif()
list(APPEND _tr_fxplot_libs
    -lXm
    X11::Xt
    X11::X11
)
if(APPLE)
    list(APPEND _tr_fxplot_libs -lc++abi)
else()
    list(APPEND _tr_fxplot_libs rt)
endif()
target_link_libraries(trick-fxplot PRIVATE ${_tr_fxplot_libs})
# UDUNITS_HOME may be a non-system prefix; keep the build-time rpath to
# libudunits2 in the installed binary (CMake drops build rpath at install by
# default) instead of the makefile's literal -Wl,-rpath.
set_target_properties(trick-fxplot PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)

install(TARGETS trick-fxplot RUNTIME DESTINATION bin)
