# libDPM.a — ported from this directory's own makefile (the authority).
set(DPM_SRC
    DPM_attribute.cpp
    DPM_axis.cpp
    DPM_column.cpp
    DPM_component.cpp
    DPM_curve.cpp
    DPM_extfn.cpp
    DPM_inputs.cpp
    DPM_measurement.cpp
    DPM_outputs.cpp
    DPM_page.cpp
    DPM_parse_tree.cpp
    DPM_product.cpp
    DPM_relation.cpp
    DPM_run.cpp
    DPM_session.cpp
    DPM_table.cpp
    DPM_time_constraints.cpp
    DPM_var.cpp
)

add_library(DPM STATIC ${DPM_SRC})
target_include_directories(DPM PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
    ${CMAKE_SOURCE_DIR}/include
    ${LIBXML2_INCLUDE_DIR}
)
separate_arguments(_tr_dpm_udunits_includes NATIVE_COMMAND "${UDUNITS_INCLUDES}")
target_compile_options(DPM PRIVATE ${_tr_dpm_udunits_includes})
