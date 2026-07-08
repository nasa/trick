# libDPC.a — ported from this directory's own makefile (the authority).
set(DPC_SRC
    DPC_TimeCstrDataStream.cpp
    DPC_UnitConvDataStream.cpp
    DPC_datastream_supplier.cpp
    DPC_delta_curve.cpp
    DPC_delta_plot.cpp
    DPC_page.cpp
    DPC_plot.cpp
    DPC_product.cpp
    DPC_standard_plot.cpp
    DPC_std_curve.cpp
    DPC_table.cpp
)

add_library(DPC STATIC ${DPC_SRC})
target_include_directories(DPC PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
    ${CMAKE_SOURCE_DIR}/include
)
# PRIVATE: libxml2 headers are a compile-only need (static archive → no link
# propagation); LibXml2::LibXml2 supplies the include dirs.
target_link_libraries(DPC PRIVATE LibXml2::LibXml2)
separate_arguments(_tr_dpc_udunits_includes NATIVE_COMMAND "${UDUNITS_INCLUDES}")
target_compile_options(DPC PRIVATE ${_tr_dpc_udunits_includes})
