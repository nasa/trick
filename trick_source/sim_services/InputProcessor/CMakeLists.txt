
set( INPUT_PROCESSOR_SRC
  IPPython
  IPPythonEvent
  InputProcessor
  MTV
  MTV_c_intf
  input_processor_ext
)

add_library( input_processor_objs OBJECT ${INPUT_PROCESSOR_SRC})
target_include_directories( input_processor_objs PUBLIC ${PYTHON_INCLUDE_DIRS} )

