add_subdirectory(trick_utils)

if(TRICK_ENABLE_ER7_UTILS)
    add_subdirectory(er7_utils)
endif()

add_subdirectory(codegen/Interface_Code_Gen)
