set( INPUT_PROCESSOR_SRC
  IPPython.cpp
  IPPythonEvent.cpp
  InputProcessor.cpp
  MTV.cpp
  MTV_c_intf.cpp
  input_processor_ext.cpp
)

add_library( input_processor_objs OBJECT ${INPUT_PROCESSOR_SRC})
target_link_libraries( input_processor_objs PUBLIC trick_build_flags )
target_include_directories( input_processor_objs PRIVATE ${Python3_INCLUDE_DIRS} )
