# libtrick_pyip.a: SWIG-generated Trick/Python interface code, ported from
# this directory's Makefile (the authority — see A3/A4).

find_package(SWIG 3.0 REQUIRED)

# D3: CMake writes nothing into the source tree — .py wrappers land in the
# build tree; Phase 3's `stage` install target is what assembles a runnable
# TRICK_HOME (including share/trick/swig/*.py) for sim-flow validation.
set(TRICK_SWIG_OUTDIR ${CMAKE_BINARY_DIR}/share/trick/swig)
file(MAKE_DIRECTORY ${TRICK_SWIG_OUTDIR})

set(_tr_swig_defs "")
if(HDF5_HOME)
    list(APPEND _tr_swig_defs -DHDF5)
endif()
if(GSL_HOME)
    list(APPEND _tr_swig_defs -D_HAVE_GSL)
endif()
if(TRICK_USE_ER7_UTILS)
    list(APPEND _tr_swig_defs -DUSE_ER7_UTILS_INTEGRATORS)
    if(EXISTS ${CMAKE_SOURCE_DIR}/trick_source/er7_utils/CheckpointHelper)
        list(APPEND _tr_swig_defs -DUSE_ER7_UTILS_CHECKPOINTHELPER)
    endif()
endif()
if(TRICK_CIVETWEB_HOME)
    list(APPEND _tr_swig_defs -DUSE_CIVETWEB)
endif()

function(trick_add_swig_wrap name)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${name}_wrap.cpp
        COMMAND ${SWIG_EXECUTABLE} ${_tr_swig_defs}
            -I${CMAKE_SOURCE_DIR}/trick_source -I${CMAKE_SOURCE_DIR}/include -I${CMAKE_SOURCE_DIR}
            -c++ -python -includeall -ignoremissing -w201,362,389,451
            -o ${CMAKE_CURRENT_BINARY_DIR}/${name}_wrap.cpp
            -outdir ${TRICK_SWIG_OUTDIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/${name}.i
        MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/${name}.i
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/extra_functions.i ${CMAKE_CURRENT_SOURCE_DIR}/units_attach.i
        COMMENT "SWIG ${name}.i"
        VERBATIM
    )
endfunction()

trick_add_swig_wrap(sim_services)
trick_add_swig_wrap(swig_double)
trick_add_swig_wrap(swig_int)
trick_add_swig_wrap(swig_ref)

add_library( trick_pyip STATIC
    ${CMAKE_CURRENT_BINARY_DIR}/sim_services_wrap.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/swig_double_wrap.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/swig_int_wrap.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/swig_ref_wrap.cpp
    PrimitiveAttributesMap.cpp
    swig_convert_units.cpp
    swig_global_vars.cpp
    $<TARGET_OBJECTS:input_processor_objs>
)
target_link_libraries( trick_pyip PUBLIC trick_build_flags )
target_include_directories( trick_pyip PRIVATE ${Python3_INCLUDE_DIRS} )
set_target_properties( trick_pyip PROPERTIES ARCHIVE_OUTPUT_NAME trick_pyip)

# Makefile: trick_swig/Makefile:28-34 — SWIG-generated code triggers warnings
# the rest of the build treats as errors elsewhere; suppress the same set.
set(_tr_swig_wrap_opts -Wno-redundant-decls -Wno-shadow -Wno-unused-parameter -Wno-missing-field-initializers)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    list(APPEND _tr_swig_wrap_opts -Wno-self-assign -Wno-sometimes-uninitialized)
else()
    list(APPEND _tr_swig_wrap_opts -Wno-cast-function-type -Wno-ignored-qualifiers -Wno-delete-non-virtual-dtor)
endif()
set_source_files_properties(
    ${CMAKE_CURRENT_BINARY_DIR}/sim_services_wrap.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/swig_double_wrap.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/swig_int_wrap.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/swig_ref_wrap.cpp
    PROPERTIES COMPILE_OPTIONS "${_tr_swig_wrap_opts}"
)
