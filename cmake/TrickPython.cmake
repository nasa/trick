# Python detection for config_user.mk (D5): capture literal python-config
# output strings rather than relying on find_package(Python3) values, since
# sims' historical link lines include platform extras that Python3_LIBRARIES
# does not reproduce. Mirrors autoconf/configure.ac:127-187 exactly,
# including search order and the `--embed` / newline-stripping behavior.
#
# Honors the PYTHON_VERSION cache variable (e.g. "3" looks for python3),
# equivalent to configure's PYTHON_VERSION influential env var.
#
# Exports: PYTHON, PYTHON_CONFIG, PYTHON_CPPFLAGS, PYTHON_LIBS

find_program(PYTHON
    NAMES python${PYTHON_VERSION} python python3
)
if(NOT PYTHON)
    message(FATAL_ERROR "could not find python<version> python or python3. Please install the python development package")
endif()

execute_process(
    COMMAND ${PYTHON} -c "import sys; print(str(sys.version_info[0])+\".\"+str(sys.version_info[1]))"
    OUTPUT_VARIABLE PYTHON_MAJORMINOR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND ${PYTHON} -c "import sys; print(str(sys.version_info[0]))"
    OUTPUT_VARIABLE PYTHON_MAJOR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

find_program(PYTHON_CONFIG
    NAMES python${PYTHON_MAJORMINOR}-config python${PYTHON_MAJOR}-config python${PYTHON_VERSION}-config python-config
)
if(NOT PYTHON_CONFIG)
    message(FATAL_ERROR "could not find python<major>-config python<major.minor>-config or python-config. Please install the python development package")
endif()

set(_tr_python_libs_command --ldflags --libs)
if(NOT PYTHON_MAJORMINOR VERSION_LESS "3.8")
    list(APPEND _tr_python_libs_command --embed)
endif()

execute_process(
    COMMAND ${PYTHON_CONFIG} --includes
    OUTPUT_VARIABLE PYTHON_CPPFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
string(REPLACE "-I" "-isystem" PYTHON_CPPFLAGS "${PYTHON_CPPFLAGS}")

execute_process(
    COMMAND ${PYTHON_CONFIG} ${_tr_python_libs_command}
    OUTPUT_VARIABLE PYTHON_LIBS
)
string(REPLACE "\r" " " PYTHON_LIBS "${PYTHON_LIBS}")
string(REPLACE "\n" " " PYTHON_LIBS "${PYTHON_LIBS}")
string(STRIP "${PYTHON_LIBS}" PYTHON_LIBS)

# Always empty today (see A1) — kept as a named, substituted variable for
# parity with config_user.mk.in's contract.
set(PYTHON_EXTRA_LIBS "")
