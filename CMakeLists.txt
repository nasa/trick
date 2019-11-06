cmake_minimum_required(VERSION 2.8)
project(trick)
set(TRICK_MAJOR 19)
set(TRICK_MINOR 1)
set(TRICK_TINY dev)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib64)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib64)

include(UseJava)
find_package(Java REQUIRED)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(LLVM 3.4 REQUIRED)
find_package(Perl REQUIRED)
find_package(PythonInterp REQUIRED)
find_package(PythonLibs REQUIRED)
find_package(SWIG REQUIRED)
find_package(Tee REQUIRED)
find_package(UDUNITS2 REQUIRED)
find_package(HDF5)
find_package(GSL)

add_definitions( -DTRICK_VER=${TRICK_MAJOR} )
add_definitions( -DUSE_ER7_UTILS_INTEGRATORS)

if(GSL_FOUND)
    add_definitions( -D_HAVE_GSL)
endif()

file(COPY bin DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY include DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY libexec DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY share DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/mongoose)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/io_src)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/java)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lex_yacc)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/mongoose)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/swig)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/share/trick/makefiles/config_user.mk.in ${CMAKE_CURRENT_BINARY_DIR}/share/trick/makefiles/config_user.mk)

file(GLOB_RECURSE ER7_UTIL_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/er7_utils ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/er7_utils/*.hh)
foreach ( infile ${ER7_UTIL_HEADERS} )
    get_filename_component(dir ${infile} DIRECTORY)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/er7_utils/${infile} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/include/er7_utils/${dir}")
endforeach(infile)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/include/mongoose/mongoose.h
  COMMAND curl --retry 4 -O https://raw.githubusercontent.com/cesanta/mongoose/6.16/mongoose.h
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/mongoose
)

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/mongoose/mongoose.c
  COMMAND curl --retry 4 -O https://raw.githubusercontent.com/cesanta/mongoose/6.16/mongoose.c
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/mongoose
)

###############################################################
# mongoose lib
###############################################################

add_library(mongoose STATIC ${CMAKE_CURRENT_BINARY_DIR}/mongoose/mongoose.c)
target_include_directories( mongoose PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include/mongoose )

###############################################################
# ICG
###############################################################

set ( ICG_SRC
  trick_source/codegen/Interface_Code_Gen/ClassTemplateVisitor
  trick_source/codegen/Interface_Code_Gen/ClassValues
  trick_source/codegen/Interface_Code_Gen/ClassVisitor
  trick_source/codegen/Interface_Code_Gen/CommentSaver
  trick_source/codegen/Interface_Code_Gen/ConstructValues
  trick_source/codegen/Interface_Code_Gen/EnumValues
  trick_source/codegen/Interface_Code_Gen/EnumVisitor
  trick_source/codegen/Interface_Code_Gen/FieldDescription
  trick_source/codegen/Interface_Code_Gen/FieldVisitor
  trick_source/codegen/Interface_Code_Gen/FindTrickICG
  trick_source/codegen/Interface_Code_Gen/HeaderSearchDirs
  trick_source/codegen/Interface_Code_Gen/ICGASTConsumer
  trick_source/codegen/Interface_Code_Gen/PrintAttributes
  trick_source/codegen/Interface_Code_Gen/PrintFileContents10
  trick_source/codegen/Interface_Code_Gen/PrintFileContentsBase
  trick_source/codegen/Interface_Code_Gen/TranslationUnitVisitor
  trick_source/codegen/Interface_Code_Gen/TypedefVisitor
  trick_source/codegen/Interface_Code_Gen/Utilities
  trick_source/codegen/Interface_Code_Gen/VariableVisitor
  trick_source/codegen/Interface_Code_Gen/main
  trick_source/sim_services/UdUnits/map_trick_units_to_udunits
)

add_definitions( -g -DTRICK_VERSION="${TRICK_MAJOR}.${TRICK_MINOR}.${TRICK_TINY}" -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS)
add_definitions( -DLIBCLANG_MAJOR=${LLVM_VERSION_MAJOR} -DLIBCLANG_MINOR=${LLVM_VERSION_MINOR})
add_definitions( -DLIBCLANG_MAJOR=7 -DLIBCLANG_MINOR=0)

add_executable( trick-ICG ${ICG_SRC} )
target_include_directories( trick-ICG PUBLIC ${UDUNITS2_INCLUDES} )
target_include_directories( trick-ICG PUBLIC ${LLVM_INCLUDE_DIR} )
target_include_directories( trick-ICG PUBLIC include )
set_property(SOURCE trick-ICG APPEND PROPERTY OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/include/mongoose/mongoose.h)

target_link_libraries( trick-ICG
    ${LLVM_LDFLAGS}
    ${LLVM_LIBRARIES}
    -lclangFrontend
    -lclangDriver
    -lclangSerialization
    -lclangParse
    -lclangSema
    -lclangAnalysis
    -lclangEdit
    -lclangAST
    -lclangLex
    -lclangBasic
    ${UDUNITS2_LIBRARIES}
)

###############################################################
# Trick source
###############################################################

set( SS_SRC
  trick_source/sim_services/CheckPointAgent/CheckPointAgent
  trick_source/sim_services/CheckPointAgent/ChkPtParseContext
  trick_source/sim_services/CheckPointAgent/ClassicCheckPointerAgent
  trick_source/sim_services/CheckPointAgent/PythonPrint
  trick_source/sim_services/CheckPointRestart/CheckPointRestart
  trick_source/sim_services/CheckPointRestart/CheckPointRestart_c_intf
  trick_source/sim_services/CheckPointRestart/next_attr_name
  trick_source/sim_services/CheckPointRestart/stl_type_name_convert
  trick_source/sim_services/Clock/BC635Clock
  trick_source/sim_services/Clock/Clock
  trick_source/sim_services/Clock/GetTimeOfDayClock
  trick_source/sim_services/Clock/TPROCTEClock
  trick_source/sim_services/Clock/clock_c_intf
  trick_source/sim_services/Collect/collect
  trick_source/sim_services/CommandLineArguments/CommandLineArguments
  trick_source/sim_services/CommandLineArguments/command_line_c_intf
  trick_source/sim_services/DMTCP/DMTCP
  trick_source/sim_services/DMTCP/dmtcp_checkpoint_c_intf
  trick_source/sim_services/DataRecord/DRAscii
  trick_source/sim_services/DataRecord/DRBinary
  trick_source/sim_services/DataRecord/DRHDF5
  trick_source/sim_services/DataRecord/DataRecordDispatcher
  trick_source/sim_services/DataRecord/DataRecordGroup
  trick_source/sim_services/DataRecord/data_record_utilities
  trick_source/sim_services/DebugPause/DebugPause
  trick_source/sim_services/DebugPause/DebugPause_c_intf
  trick_source/sim_services/EchoJobs/EchoJobs
  trick_source/sim_services/EchoJobs/EchoJobs_c_intf
  trick_source/sim_services/Environment/Environment
  trick_source/sim_services/Environment/Environment_c_intf
  trick_source/sim_services/EventManager/EventInstrument
  trick_source/sim_services/EventManager/EventManager
  trick_source/sim_services/EventManager/EventManager_c_intf
  trick_source/sim_services/EventManager/EventProcessor
  trick_source/sim_services/Executive/Executive
  trick_source/sim_services/Executive/ExecutiveException
  trick_source/sim_services/Executive/Executive_add_depends_on_job
  trick_source/sim_services/Executive/Executive_add_jobs_to_queue
  trick_source/sim_services/Executive/Executive_add_scheduled_job_class
  trick_source/sim_services/Executive/Executive_add_sim_object
  trick_source/sim_services/Executive/Executive_advance_sim_time
  trick_source/sim_services/Executive/Executive_c_intf
  trick_source/sim_services/Executive/Executive_call_default_data
  trick_source/sim_services/Executive/Executive_call_initialization
  trick_source/sim_services/Executive/Executive_call_input_processor
  trick_source/sim_services/Executive/Executive_check_all_job_cycle_times
  trick_source/sim_services/Executive/Executive_check_all_jobs_handled
  trick_source/sim_services/Executive/Executive_checkpoint
  trick_source/sim_services/Executive/Executive_clear_scheduled_queues
  trick_source/sim_services/Executive/Executive_create_threads
  trick_source/sim_services/Executive/Executive_fpe_handler
  trick_source/sim_services/Executive/Executive_freeze
  trick_source/sim_services/Executive/Executive_freeze_loop
  trick_source/sim_services/Executive/Executive_get_curr_job
  trick_source/sim_services/Executive/Executive_get_job
  trick_source/sim_services/Executive/Executive_get_job_cycle
  trick_source/sim_services/Executive/Executive_get_process_id
  trick_source/sim_services/Executive/Executive_get_sim_time
  trick_source/sim_services/Executive/Executive_init
  trick_source/sim_services/Executive/Executive_init_freeze_scheduled
  trick_source/sim_services/Executive/Executive_init_signal_handlers
  trick_source/sim_services/Executive/Executive_instrument_job
  trick_source/sim_services/Executive/Executive_isThreadReadyToRun
  trick_source/sim_services/Executive/Executive_loop
  trick_source/sim_services/Executive/Executive_loop_multi_thread
  trick_source/sim_services/Executive/Executive_loop_single_thread
  trick_source/sim_services/Executive/Executive_post_checkpoint
  trick_source/sim_services/Executive/Executive_process_sim_args
  trick_source/sim_services/Executive/Executive_register_scheduler
  trick_source/sim_services/Executive/Executive_remove_jobs
  trick_source/sim_services/Executive/Executive_remove_sim_object
  trick_source/sim_services/Executive/Executive_restart
  trick_source/sim_services/Executive/Executive_run
  trick_source/sim_services/Executive/Executive_scheduled_thread_sync
  trick_source/sim_services/Executive/Executive_set_job_cycle
  trick_source/sim_services/Executive/Executive_set_job_onoff
  trick_source/sim_services/Executive/Executive_set_simobject_onoff
  trick_source/sim_services/Executive/Executive_set_thread_amf_cycle_time
  trick_source/sim_services/Executive/Executive_set_thread_async_wait
  trick_source/sim_services/Executive/Executive_set_thread_cpu_affinity
  trick_source/sim_services/Executive/Executive_set_thread_enabled
  trick_source/sim_services/Executive/Executive_set_thread_priority
  trick_source/sim_services/Executive/Executive_set_thread_process_type
  trick_source/sim_services/Executive/Executive_set_thread_rt_semaphore
  trick_source/sim_services/Executive/Executive_set_time_tic_value
  trick_source/sim_services/Executive/Executive_shutdown
  trick_source/sim_services/Executive/Executive_signal_handler
  trick_source/sim_services/Executive/Executive_stop
  trick_source/sim_services/Executive/Executive_terminate
  trick_source/sim_services/Executive/Executive_thread_sync
  trick_source/sim_services/Executive/Executive_write_s_job_execution
  trick_source/sim_services/Executive/Executive_write_s_run_summary
  trick_source/sim_services/Executive/ThreadTrigger
  trick_source/sim_services/Executive/Threads
  trick_source/sim_services/Executive/Threads_child
  trick_source/sim_services/Executive/Threads_set_amf_cycle_tics
  trick_source/sim_services/Executive/Threads_set_async_wait
  trick_source/sim_services/Executive/Threads_set_process_type
  trick_source/sim_services/Executive/child_handler
  trick_source/sim_services/Executive/fpe_handler
  trick_source/sim_services/Executive/sig_hand
  trick_source/sim_services/ExternalApplications/ExternalApplication
  trick_source/sim_services/ExternalApplications/ExternalApplicationManager
  trick_source/sim_services/ExternalApplications/ExternalApplication_c_intf
  trick_source/sim_services/ExternalApplications/MalfunctionsTrickView
  trick_source/sim_services/ExternalApplications/MonteMonitor
  trick_source/sim_services/ExternalApplications/SimControlPanel
  trick_source/sim_services/ExternalApplications/StripChart
  trick_source/sim_services/ExternalApplications/TrickView
  trick_source/sim_services/FrameLog/FrameDataRecordGroup
  trick_source/sim_services/FrameLog/FrameLog
  trick_source/sim_services/FrameLog/FrameLog_c_intf
  trick_source/sim_services/InputProcessor/IPPython
  trick_source/sim_services/InputProcessor/IPPythonEvent
  trick_source/sim_services/InputProcessor/InputProcessor
  trick_source/sim_services/InputProcessor/MTV
  trick_source/sim_services/InputProcessor/MTV_c_intf
  trick_source/sim_services/InputProcessor/input_processor_ext
  trick_source/sim_services/Integrator/src/IntegLoopManager
  trick_source/sim_services/Integrator/src/IntegLoopScheduler
  trick_source/sim_services/Integrator/src/IntegLoopSimObject
  trick_source/sim_services/Integrator/src/Integrator
  trick_source/sim_services/Integrator/src/Integrator_C_Intf
  trick_source/sim_services/Integrator/src/getIntegrator
  trick_source/sim_services/Integrator/src/regula_falsi
  trick_source/sim_services/Integrator/src/reset_regula_falsi
  trick_source/sim_services/JITInputFile/JITEvent
  trick_source/sim_services/JITInputFile/JITInputFile
  trick_source/sim_services/JITInputFile/jit_input_file_c_intf
  trick_source/sim_services/JSONVariableServer/JSONVariableServer
  trick_source/sim_services/JSONVariableServer/JSONVariableServerThread
  trick_source/sim_services/MasterSlave/MSSharedMem
  trick_source/sim_services/MasterSlave/MSSocket
  trick_source/sim_services/MasterSlave/Master
  trick_source/sim_services/MasterSlave/Slave
  trick_source/sim_services/MemoryManager/ADefParseContext
  trick_source/sim_services/MemoryManager/MemoryManager
  trick_source/sim_services/MemoryManager/MemoryManager_C_Intf
  trick_source/sim_services/MemoryManager/MemoryManager_JSON_Intf
  trick_source/sim_services/MemoryManager/MemoryManager_add_attr_info
  trick_source/sim_services/MemoryManager/MemoryManager_add_checkpoint_alloc_dependency
  trick_source/sim_services/MemoryManager/MemoryManager_add_shared_library_symbols
  trick_source/sim_services/MemoryManager/MemoryManager_add_template_name_trans
  trick_source/sim_services/MemoryManager/MemoryManager_add_var
  trick_source/sim_services/MemoryManager/MemoryManager_alloc_depends
  trick_source/sim_services/MemoryManager/MemoryManager_alloc_info_map
  trick_source/sim_services/MemoryManager/MemoryManager_clear_memory
  trick_source/sim_services/MemoryManager/MemoryManager_declare_var
  trick_source/sim_services/MemoryManager/MemoryManager_delete_var
  trick_source/sim_services/MemoryManager/MemoryManager_get_enumerated
  trick_source/sim_services/MemoryManager/MemoryManager_get_size
  trick_source/sim_services/MemoryManager/MemoryManager_get_stl_dependencies
  trick_source/sim_services/MemoryManager/MemoryManager_get_type_attributes
  trick_source/sim_services/MemoryManager/MemoryManager_io_src_intf
  trick_source/sim_services/MemoryManager/MemoryManager_is_alloced
  trick_source/sim_services/MemoryManager/MemoryManager_make_declaration
  trick_source/sim_services/MemoryManager/MemoryManager_make_reference_attr
  trick_source/sim_services/MemoryManager/MemoryManager_map_external_object
  trick_source/sim_services/MemoryManager/MemoryManager_realloc
  trick_source/sim_services/MemoryManager/MemoryManager_ref_allocate
  trick_source/sim_services/MemoryManager/MemoryManager_ref_assignment
  trick_source/sim_services/MemoryManager/MemoryManager_ref_attributes
  trick_source/sim_services/MemoryManager/MemoryManager_ref_dim
  trick_source/sim_services/MemoryManager/MemoryManager_ref_name
  trick_source/sim_services/MemoryManager/MemoryManager_ref_name_from_address
  trick_source/sim_services/MemoryManager/MemoryManager_ref_var
  trick_source/sim_services/MemoryManager/MemoryManager_restore
  trick_source/sim_services/MemoryManager/MemoryManager_restore_stls
  trick_source/sim_services/MemoryManager/MemoryManager_set_checkpointagent
  trick_source/sim_services/MemoryManager/MemoryManager_set_debug_level
  trick_source/sim_services/MemoryManager/MemoryManager_strdup
  trick_source/sim_services/MemoryManager/MemoryManager_write_checkpoint
  trick_source/sim_services/MemoryManager/MemoryManager_write_var
  trick_source/sim_services/MemoryManager/RefParseContext
  trick_source/sim_services/MemoryManager/addr_bitfield
  trick_source/sim_services/MemoryManager/extract_bitfield
  trick_source/sim_services/MemoryManager/extract_unsigned_bitfield
  trick_source/sim_services/MemoryManager/follow_address_path
  trick_source/sim_services/MemoryManager/insert_bitfield
  trick_source/sim_services/MemoryManager/parameter_types
  trick_source/sim_services/MemoryManager/ref_free
  trick_source/sim_services/MemoryManager/ref_to_value
  trick_source/sim_services/MemoryManager/trickTypeCharString
  trick_source/sim_services/MemoryManager/vval
  trick_source/sim_services/MemoryManager/wcs_ext
  trick_source/sim_services/Message/MessageCout
  trick_source/sim_services/Message/MessageFile
  trick_source/sim_services/Message/MessageLCout
  trick_source/sim_services/Message/MessagePublisher
  trick_source/sim_services/Message/MessageSubscriber
  trick_source/sim_services/Message/MessageTCDevice
  trick_source/sim_services/Message/MessageThreadedCout
  trick_source/sim_services/Message/Message_c_intf
  trick_source/sim_services/Message/PlaybackFile
  trick_source/sim_services/Message/message_publish_standalone
  trick_source/sim_services/MonteCarlo/MonteCarlo
  trick_source/sim_services/MonteCarlo/MonteCarlo_c_intf
  trick_source/sim_services/MonteCarlo/MonteCarlo_dispatch_run_to_slave
  trick_source/sim_services/MonteCarlo/MonteCarlo_dryrun
  trick_source/sim_services/MonteCarlo/MonteCarlo_execute_monte
  trick_source/sim_services/MonteCarlo/MonteCarlo_funcs
  trick_source/sim_services/MonteCarlo/MonteCarlo_initialize_sockets
  trick_source/sim_services/MonteCarlo/MonteCarlo_master
  trick_source/sim_services/MonteCarlo/MonteCarlo_master_file_io
  trick_source/sim_services/MonteCarlo/MonteCarlo_master_init
  trick_source/sim_services/MonteCarlo/MonteCarlo_master_shutdown
  trick_source/sim_services/MonteCarlo/MonteCarlo_receive_results
  trick_source/sim_services/MonteCarlo/MonteCarlo_run_queue
  trick_source/sim_services/MonteCarlo/MonteCarlo_slave
  trick_source/sim_services/MonteCarlo/MonteCarlo_slave_funcs
  trick_source/sim_services/MonteCarlo/MonteCarlo_slave_init
  trick_source/sim_services/MonteCarlo/MonteCarlo_slave_process_run
  trick_source/sim_services/MonteCarlo/MonteCarlo_spawn_slaves
  trick_source/sim_services/MonteCarlo/MonteVarCalculated
  trick_source/sim_services/MonteCarlo/MonteVarFile
  trick_source/sim_services/MonteCarlo/MonteVarFixed
  trick_source/sim_services/MonteCarlo/MonteVarRandom
  trick_source/sim_services/MonteCarlo/StlRandomGenerator
  trick_source/sim_services/RealtimeInjector/RtiEvent
  trick_source/sim_services/RealtimeInjector/RtiExec
  trick_source/sim_services/RealtimeInjector/RtiList
  trick_source/sim_services/RealtimeInjector/RtiStager
  trick_source/sim_services/RealtimeSync/RealtimeSync
  trick_source/sim_services/RealtimeSync/RealtimeSync_c_intf
  trick_source/sim_services/ScheduledJobQueue/ScheduledJobQueue
  trick_source/sim_services/ScheduledJobQueue/ScheduledJobQueueInstrument
  trick_source/sim_services/Scheduler/Scheduler
  trick_source/sim_services/Sie/AttributesMap
  trick_source/sim_services/Sie/EnumAttributesMap
  trick_source/sim_services/Sie/Sie
  trick_source/sim_services/Sie/sie_c_intf
  trick_source/sim_services/SimObject/JobData
  trick_source/sim_services/SimObject/SimObject
  trick_source/sim_services/SimTime/SimTime
  trick_source/sim_services/SimTime/SimTime_c_intf
  trick_source/sim_services/ThreadBase/ThreadBase
  trick_source/sim_services/Timer/ITimer
  trick_source/sim_services/Timer/Timer
  trick_source/sim_services/Timer/it_handler
  trick_source/sim_services/UdUnits/UdUnits
  trick_source/sim_services/UdUnits/map_trick_units_to_udunits
  trick_source/sim_services/UnitTest/UnitTest
  trick_source/sim_services/UnitTest/UnitTest_c_intf
  trick_source/sim_services/UnitsMap/UnitsMap
  trick_source/sim_services/VariableServer/VariableReference
  trick_source/sim_services/VariableServer/VariableServer
  trick_source/sim_services/VariableServer/VariableServerListenThread
  trick_source/sim_services/VariableServer/VariableServerThread
  trick_source/sim_services/VariableServer/VariableServerThread_commands
  trick_source/sim_services/VariableServer/VariableServerThread_connect
  trick_source/sim_services/VariableServer/VariableServerThread_copy_data
  trick_source/sim_services/VariableServer/VariableServerThread_copy_sim_data
  trick_source/sim_services/VariableServer/VariableServerThread_create_socket
  trick_source/sim_services/VariableServer/VariableServerThread_freeze_init
  trick_source/sim_services/VariableServer/VariableServerThread_loop
  trick_source/sim_services/VariableServer/VariableServerThread_restart
  trick_source/sim_services/VariableServer/VariableServerThread_write_data
  trick_source/sim_services/VariableServer/VariableServerThread_write_stdio
  trick_source/sim_services/VariableServer/VariableServer_copy_data_freeze
  trick_source/sim_services/VariableServer/VariableServer_copy_data_freeze_scheduled
  trick_source/sim_services/VariableServer/VariableServer_copy_data_scheduled
  trick_source/sim_services/VariableServer/VariableServer_copy_data_top
  trick_source/sim_services/VariableServer/VariableServer_default_data
  trick_source/sim_services/VariableServer/VariableServer_freeze_init
  trick_source/sim_services/VariableServer/VariableServer_get_next_freeze_call_time
  trick_source/sim_services/VariableServer/VariableServer_get_next_sync_call_time
  trick_source/sim_services/VariableServer/VariableServer_get_var_server_port
  trick_source/sim_services/VariableServer/VariableServer_init
  trick_source/sim_services/VariableServer/VariableServer_restart
  trick_source/sim_services/VariableServer/VariableServer_shutdown
  trick_source/sim_services/VariableServer/exit_var_thread
  trick_source/sim_services/VariableServer/var_server_ext
  trick_source/sim_services/VariableServer/vs_format_ascii
  trick_source/sim_services/Zeroconf/Zeroconf
  trick_source/sim_services/mains/master
  trick_source/trick_swig/PrimitiveAttributesMap
  trick_source/trick_swig/swig_convert_units
  trick_source/trick_swig/swig_global_vars
  trick_source/web/HttpServer/src/VariableServerSession
  trick_source/web/HttpServer/src/VariableServerVariable
  trick_source/web/HttpServer/src/WebServer
  trick_source/web/HttpServer/src/http_GET_handlers
  trick_source/web/HttpServer/src/simpleJSON
)

set( LEX_YACC_SRC
  ${CMAKE_CURRENT_BINARY_DIR}/lex_yacc/adef_parser.lex
  ${CMAKE_CURRENT_BINARY_DIR}/lex_yacc/adef_parser.tab
  ${CMAKE_CURRENT_BINARY_DIR}/lex_yacc/input_parser.lex
  ${CMAKE_CURRENT_BINARY_DIR}/lex_yacc/input_parser.tab
  ${CMAKE_CURRENT_BINARY_DIR}/lex_yacc/ref_parser.lex
  ${CMAKE_CURRENT_BINARY_DIR}/lex_yacc/ref_parser.tab
)

add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lex_yacc/adef_parser.lex.cpp
 COMMAND ${FLEX_EXECUTABLE} -d -o ${CMAKE_CURRENT_BINARY_DIR}/lex_yacc/adef_parser.lex.cpp ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/sim_services/MemoryManager/adef_parser.l
 MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/sim_services/MemoryManager/adef_parser.l
)
add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lex_yacc/adef_parser.tab.cpp ${CMAKE_CURRENT_BINARY_DIR}/lex_yacc/adef_parser.tab.hpp
 COMMAND ${BISON_EXECUTABLE} -d -o ${CMAKE_CURRENT_BINARY_DIR}/lex_yacc/adef_parser.tab.cpp ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/sim_services/MemoryManager/adef_parser.y
 MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/sim_services/MemoryManager/adef_parser.y
)
add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lex_yacc/input_parser.lex.cpp
 COMMAND ${FLEX_EXECUTABLE} -d -o ${CMAKE_CURRENT_BINARY_DIR}/lex_yacc/input_parser.lex.cpp ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/sim_services/CheckPointAgent/input_parser.l
 MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/sim_services/CheckPointAgent/input_parser.l
)
add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lex_yacc/input_parser.tab.cpp ${CMAKE_CURRENT_BINARY_DIR}/lex_yacc/input_parser.tab.hpp
 COMMAND ${BISON_EXECUTABLE} -d -o ${CMAKE_CURRENT_BINARY_DIR}/lex_yacc/input_parser.tab.cpp ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/sim_services/CheckPointAgent/input_parser.y
 MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/sim_services/CheckPointAgent/input_parser.y
)
add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lex_yacc/ref_parser.lex.cpp
 COMMAND ${FLEX_EXECUTABLE} -d -o ${CMAKE_CURRENT_BINARY_DIR}/lex_yacc/ref_parser.lex.cpp ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/sim_services/MemoryManager/ref_parser.l
 MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/sim_services/MemoryManager/ref_parser.l
)
add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lex_yacc/ref_parser.tab.cpp ${CMAKE_CURRENT_BINARY_DIR}/lex_yacc/ref_parser.tab.hpp
 COMMAND ${BISON_EXECUTABLE} -d -o ${CMAKE_CURRENT_BINARY_DIR}/lex_yacc/ref_parser.tab.cpp ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/sim_services/MemoryManager/ref_parser.y
 MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/sim_services/MemoryManager/ref_parser.y
)

set( TRICK_UTILS_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/tc_accept
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/tc_blockio
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/tc_broadcast_conninfo
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/tc_clock_init
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/tc_clock_time
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/tc_connect
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/tc_dev_copy
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/tc_disconnect
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/tc_error
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/tc_init
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/tc_init_mcast_client
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/tc_init_mcast_server
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/tc_init_udp_client
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/tc_init_udp_server
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/tc_isValid
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/tc_listen
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/tc_multiconnect
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/tc_pending
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/tc_read
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/tc_read_byteswap
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/tc_set_blockio
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/tc_write
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/tc_write_byteswap
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/trick_bswap_buffer
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/trick_byteswap
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/comm/src/trick_error_hndlr
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/interpolator/src/Interpolator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/LUD_inv
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/LUT_inv
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/LU_bksb
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/LU_dcmp
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/RodriguesRotation
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dLU_Choleski
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dLU_solver
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dS_function
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/deuler_123
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/deuler_123_quat
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/deuler_132
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/deuler_132_quat
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/deuler_213
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/deuler_213_quat
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/deuler_231
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/deuler_231_quat
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/deuler_312
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/deuler_312_quat
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/deuler_321
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/deuler_321_quat
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dm_add
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dm_copy
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dm_ident
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dm_init
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dm_invert
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dm_invert_symm
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dm_orthonormal
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dm_print
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dm_scale
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dm_sub
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dm_trans
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dmtxm
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dmtxmt
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dmtxv
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dmxm
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dmxmt
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dmxv
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/drandom_gaussian
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dsingle_axis_rot
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dv_add
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dv_copy
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dv_cross
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dv_dot
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dv_init
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dv_mag
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dv_norm
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dv_print
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dv_scale
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dv_skew
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dv_store
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dv_sub
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dvxm
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dvxv_add
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/dvxv_sub
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/eigen_hh_red
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/eigen_jacobi
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/eigen_jacobi_4
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/eigen_ql
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/euler_matrix
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/euler_quat
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/gauss_rnd_bell
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/gauss_rnd_pseudo
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/mat_copy
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/mat_permute
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/mat_print
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/mat_to_quat
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/mat_trans
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/matxmat
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/matxtrans
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/matxvec
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/quat_mult
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/quat_norm
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/quat_norm_integ
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/quat_to_mat
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/rand_num
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/roundoff
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/tm_print_error
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/transxmat
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/transxtrans
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/transxvec
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/trick_gsl_rand
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/trns_fnct_1o
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/trns_fnct_2o
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/uniform_rnd_1
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/uniform_rnd_triple
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/vec_print
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/math/src/wave_form
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/shm/src/tsm_disconnect
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/shm/src/tsm_init
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/shm/src/tsm_init_with_lock
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/shm/src/tsm_reconnect
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/trick_adt/src/MapStrToPtr
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/trick_adt/src/bst
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/trick_adt/src/bubble_sort
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/trick_adt/src/dllist
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/trick_adt/src/lqueue
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/trick_adt/src/lstack
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/trick_adt/src/record_array
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/unicode/src/unicode_utils
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/units/src/UCFn.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/units/src/Unit.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_utils/units/src/units_conv
)

set( IO_SRC
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/class_map.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_ABM_Integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_AttributesMap.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_BC635Clock.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_CheckPointAgent.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_CheckPointRestart.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_Clock.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_CommandLineArguments.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_DMTCP.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_DRAscii.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_DRBinary.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_DRHDF5.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_DataRecordDispatcher.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_DataRecordGroup.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_DebugPause.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_EchoJobs.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_EnumAttributesMap.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_Environment.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_Euler_Cromer_Integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_Euler_Integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_Event.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_EventInstrument.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_EventManager.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_EventProcessor.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_Executive.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_ExecutiveException.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_ExternalApplication.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_Flag.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_FrameDataRecordGroup.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_FrameLog.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_GetTimeOfDayClock.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_IPPython.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_IPPythonEvent.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_ITimer.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_InputProcessor.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_InstrumentBase.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_IntegLoopManager.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_IntegLoopScheduler.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_IntegLoopSimObject.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_Integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_JITEvent.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_JITInputFile.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_JSONVariableServer.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_JSONVariableServerThread.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_JobData.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_MM4_Integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_MSConnect.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_MSSharedMem.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_MSSocket.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_MTV.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_MalfunctionsTrickView.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_Master.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_MemoryManager.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_MessageCout.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_MessageFile.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_MessageLCout.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_MessagePublisher.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_MessageSubscriber.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_MessageTCDevice.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_MessageThreadedCout.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_MonteCarlo.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_MonteMonitor.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_MonteVar.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_NL2_Integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_PlaybackFile.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_RK2_Integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_RK4_Integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_RKF45_Integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_RKF78_Integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_RKG4_Integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_RealtimeSync.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_RemoteShell.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_RtiEvent.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_RtiExec.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_RtiList.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_RtiStager.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_STLInterface.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_ScheduledJobQueue.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_Scheduler.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_Sie.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_SimControlPanel.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_SimObject.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_SimTime.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_Slave.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_StripChart.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_TPROCTEClock.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_ThreadBase.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_ThreadTrigger.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_Threads.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_Timer.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_TrickView.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_UCFn.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_UdUnits.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_Unit.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_UnitTest.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_UnitsMap.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_VariableServer.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_VariableServerListenThread.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_VariableServerReference.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_VariableServerThread.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_WebServer.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_WebSocketSession.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_Zeroconf.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_abm4_first_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_abm4_integrator_constructor.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_abm4_second_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_attributes.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_base_integration_group.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_beeman_integrator_constructor.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_beeman_second_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_bogus_integration_controls.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_deletable.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_dllist.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_euler_first_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_euler_integrator_constructor.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_euler_second_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_first_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_generalized_position_derivative.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_integrable_object.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_integration_controls.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_integration_messages.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_integration_technique.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_integrator_constructor.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_integrator_constructor_factory.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_integrator_interface.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_integrator_result.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_integrator_result_merger.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_integrator_result_merger_container.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_io_alloc.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_left_quaternion_functions.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_lqueue.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_lstack.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_message_type.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_mm4_integrator_constructor.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_mm4_second_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_mm_error.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_ms_sim_mode.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_n_choose_m.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_nl2_integrator_constructor.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_nl2_second_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_parameter_types.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_position_verlet_integrator_constructor.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_position_verlet_second_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_priming_first_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_priming_integration_controls.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_priming_integrator_constructor.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_priming_second_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_rand_generator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_ratio128.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_reference.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_reference_frame.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_regula_falsi.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_rk2_heun_first_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_rk2_heun_integrator_constructor.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_rk2_heun_second_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_rk2_midpoint_first_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_rk2_midpoint_integrator_constructor.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_rk2_midpoint_second_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_rk4_first_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_rk4_integrator_constructor.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_rk4_second_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_rk4_second_order_ode_integrator_base.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_rkf45_first_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_rkf45_integrator_constructor.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_rkf45_second_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_rkf78_first_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_rkf78_integrator_constructor.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_rkf78_second_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_rkg4_first_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_rkg4_integrator_constructor.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_rkg4_second_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_second_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_sim_mode.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_single_cycle_integration_controls.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_standard_integration_controls.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_state_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_symplectic_euler_integrator_constructor.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_symplectic_euler_second_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_tc.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_time_interface.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_time_offset.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_trick_error_hndlr.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_trick_first_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_trick_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_trick_second_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_tsm.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_uint128.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_units_conv.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_value.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_var.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_variable_server_sync_types.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_velocity_verlet_integrator_constructor.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_velocity_verlet_second_order_ode_integrator.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/io_src/io_wave_form.cpp
)

set( ER7_UTILS_SRC
  trick_source/er7_utils/integration/abm4/src/abm4_first_order_ode_integrator
  trick_source/er7_utils/integration/abm4/src/abm4_integrator_constructor
  trick_source/er7_utils/integration/abm4/src/abm4_second_order_ode_integrator
  trick_source/er7_utils/integration/beeman/src/beeman_integrator_constructor
  trick_source/er7_utils/integration/beeman/src/beeman_second_order_ode_integrator
  trick_source/er7_utils/integration/core/src/base_integration_group
  trick_source/er7_utils/integration/core/src/bogus_integration_controls
  trick_source/er7_utils/integration/core/src/first_order_ode_integrator
  trick_source/er7_utils/integration/core/src/integration_controls
  trick_source/er7_utils/integration/core/src/integration_messages
  trick_source/er7_utils/integration/core/src/integrator_constructor
  trick_source/er7_utils/integration/core/src/integrator_constructor_factory
  trick_source/er7_utils/integration/core/src/integrator_result_merger
  trick_source/er7_utils/integration/core/src/integrator_result_merger_container
  trick_source/er7_utils/integration/core/src/left_quaternion_functions
  trick_source/er7_utils/integration/core/src/priming_first_order_ode_integrator
  trick_source/er7_utils/integration/core/src/priming_integration_controls
  trick_source/er7_utils/integration/core/src/priming_integrator_constructor
  trick_source/er7_utils/integration/core/src/priming_second_order_ode_integrator
  trick_source/er7_utils/integration/core/src/second_order_ode_integrator
  trick_source/er7_utils/integration/core/src/single_cycle_integration_controls
  trick_source/er7_utils/integration/core/src/standard_integration_controls
  trick_source/er7_utils/integration/euler/src/euler_first_order_ode_integrator
  trick_source/er7_utils/integration/euler/src/euler_integrator_constructor
  trick_source/er7_utils/integration/euler/src/euler_second_order_ode_integrator
  trick_source/er7_utils/integration/mm4/src/mm4_integrator_constructor
  trick_source/er7_utils/integration/mm4/src/mm4_second_order_ode_integrator
  trick_source/er7_utils/integration/nl2/src/nl2_integrator_constructor
  trick_source/er7_utils/integration/nl2/src/nl2_second_order_ode_integrator
  trick_source/er7_utils/integration/position_verlet/src/position_verlet_integrator_constructor
  trick_source/er7_utils/integration/position_verlet/src/position_verlet_second_order_ode_integrator
  trick_source/er7_utils/integration/rk2_heun/src/rk2_heun_first_order_ode_integrator
  trick_source/er7_utils/integration/rk2_heun/src/rk2_heun_integrator_constructor
  trick_source/er7_utils/integration/rk2_heun/src/rk2_heun_second_order_ode_integrator
  trick_source/er7_utils/integration/rk2_midpoint/src/rk2_midpoint_first_order_ode_integrator
  trick_source/er7_utils/integration/rk2_midpoint/src/rk2_midpoint_integrator_constructor
  trick_source/er7_utils/integration/rk2_midpoint/src/rk2_midpoint_second_order_ode_integrator
  trick_source/er7_utils/integration/rk4/src/rk4_first_order_ode_integrator
  trick_source/er7_utils/integration/rk4/src/rk4_integrator_constructor
  trick_source/er7_utils/integration/rk4/src/rk4_second_order_ode_integrator
  trick_source/er7_utils/integration/rk4/src/rk4_second_order_ode_integrator_base
  trick_source/er7_utils/integration/rkf45/src/rkf45_butcher_tableau
  trick_source/er7_utils/integration/rkf45/src/rkf45_first_order_ode_integrator
  trick_source/er7_utils/integration/rkf45/src/rkf45_integrator_constructor
  trick_source/er7_utils/integration/rkf45/src/rkf45_second_order_ode_integrator
  trick_source/er7_utils/integration/rkf78/src/rkf78_butcher_tableau
  trick_source/er7_utils/integration/rkf78/src/rkf78_first_order_ode_integrator
  trick_source/er7_utils/integration/rkf78/src/rkf78_integrator_constructor
  trick_source/er7_utils/integration/rkf78/src/rkf78_second_order_ode_integrator
  trick_source/er7_utils/integration/rkg4/src/rkg4_butcher_tableau
  trick_source/er7_utils/integration/rkg4/src/rkg4_first_order_ode_integrator
  trick_source/er7_utils/integration/rkg4/src/rkg4_integrator_constructor
  trick_source/er7_utils/integration/rkg4/src/rkg4_second_order_ode_integrator
  trick_source/er7_utils/integration/rkn4/src/rkn4_integrator_constructor
  trick_source/er7_utils/integration/rkn4/src/rkn4_second_order_ode_integrator
  trick_source/er7_utils/integration/symplectic_euler/src/symplectic_euler_integrator_constructor
  trick_source/er7_utils/integration/symplectic_euler/src/symplectic_euler_second_order_ode_integrator
  trick_source/er7_utils/integration/velocity_verlet/src/velocity_verlet_integrator_constructor
  trick_source/er7_utils/integration/velocity_verlet/src/velocity_verlet_second_order_ode_integrator
  trick_source/er7_utils/interface/src/alloc
  trick_source/er7_utils/interface/src/deletable
  trick_source/er7_utils/interface/src/message_handler
  trick_source/er7_utils/math/src/n_choose_m
  trick_source/er7_utils/math/src/ratio128
  trick_source/er7_utils/math/src/uint128
  trick_source/er7_utils/trick/integration/src/trick_integrator
)

set(ENV{TRICK_ICG_EXCLUDE} ${CMAKE_CURRENT_BINARY_DIR}/include/mongoose)
set(ICG_FLAGS -I${CMAKE_CURRENT_BINARY_DIR}/include -I${CMAKE_CURRENT_BINARY_DIR}/include/trick/compat -I${UDUNITS2_INCLUDES} -DTRICK_VER=${TRICK_MAJOR} -DUSE_ER7_UTILS_INTEGRATORS)
add_custom_command(OUTPUT ${IO_SRC}
  COMMAND TRICK_ICG_EXCLUDE=${CMAKE_CURRENT_BINARY_DIR}/include/mongoose ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/trick-ICG -force -sim_services -m -n -o ${CMAKE_CURRENT_BINARY_DIR}/io_src ${ICG_FLAGS} ${CMAKE_CURRENT_BINARY_DIR}/include/trick/files_to_ICG.hh
  DEPENDS trick-ICG
)

set( SWIG_SRC
  ${CMAKE_CURRENT_BINARY_DIR}/swig/sim_services_wrap
  ${CMAKE_CURRENT_BINARY_DIR}/swig/swig_double_wrap
  ${CMAKE_CURRENT_BINARY_DIR}/swig/swig_int_wrap
  ${CMAKE_CURRENT_BINARY_DIR}/swig//swig_ref_wrap
)

set( SWIG_SRC_BASENAME
  sim_services
  swig_double
  swig_int
  swig_ref
)

set(SWIG_FLAGS -DUSE_ER7_UTILS_INTEGRATORS -D_HAVE_GSL)
foreach ( infile ${SWIG_SRC_BASENAME} )
    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/swig/${infile}_wrap.cpp
      COMMAND ${SWIG_EXECUTABLE} ${SWIG_FLAGS} -I${CMAKE_CURRENT_SOURCE_DIR}/trick_source -I${CMAKE_CURRENT_SOURCE_DIR}/include -I${CMAKE_CURRENT_SOURCE_DIR} -c++ -python -includeall -ignoremissing -w201,362,389,451 -o $@ -outdir ${CMAKE_CURRENT_BINARY_DIR}/swig ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/trick_swig/${infile}.i
    )
endforeach(infile)


add_library( trick STATIC ${SS_SRC} ${LEX_YACC_SRC} ${TRICK_UTILS_SRC} ${IO_SRC} ${ER7_UTILS_SRC} ${SWIG_SRC})

target_include_directories( trick PUBLIC ${PYTHON_INCLUDE_DIRS} )
target_include_directories( trick PUBLIC ${UDUNITS2_INCLUDES} )
target_include_directories( trick PUBLIC include )
target_include_directories( trick PUBLIC trick_source )
target_include_directories( trick PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories( trick PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include)
target_include_directories( trick PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include/trick/compat)
target_include_directories( trick PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/trick_source/er7_utils/integration/abm4/include)
target_include_directories( trick PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/trick_source/er7_utils/integration/beeman/include)
target_include_directories( trick PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/trick_source/er7_utils/integration/core/include)
target_include_directories( trick PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/trick_source/er7_utils/integration/euler/include)
target_include_directories( trick PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/trick_source/er7_utils/integration/mm4/include)
target_include_directories( trick PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/trick_source/er7_utils/integration/nl2/include)
target_include_directories( trick PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/trick_source/er7_utils/integration/position_verlet/include)
target_include_directories( trick PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/trick_source/er7_utils/integration/rk2_heun/include)
target_include_directories( trick PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/trick_source/er7_utils/integration/rk2_midpoint/include)
target_include_directories( trick PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/trick_source/er7_utils/integration/rk4/include)
target_include_directories( trick PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/trick_source/er7_utils/integration/rkf45/include)
target_include_directories( trick PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/trick_source/er7_utils/integration/rkf78/include)
target_include_directories( trick PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/trick_source/er7_utils/integration/rkg4/include)
target_include_directories( trick PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/trick_source/er7_utils/integration/rkn4/include)
target_include_directories( trick PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/trick_source/er7_utils/integration/symplectic_euler/include)
target_include_directories( trick PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/trick_source/er7_utils/integration/velocity_verlet/include)
target_include_directories( trick PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/trick_source/er7_utils/interface/include)
target_include_directories( trick PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/trick_source/er7_utils/math/include)
target_include_directories( trick PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/trick_source/er7_utils/trick/include)
target_include_directories( trick PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/trick_source/er7_utils/trick/integration/include)

###############################################################
# Java libraries
###############################################################

set( JAVA_LIBS
 ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/bsaf-1.9.2.jar
 ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/jcommon-1.0.23.jar
 ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/jfreechart-1.0.19.jar
 ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/jfreesvg-2.1.jar
 ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/jopt-simple-4.8.jar
 ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/junit-4.12.jar
 ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/swingx-1.6.1.jar
 ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/hamcrest-core-1.3.jar
 ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/pdfbox-2.0.11.jar
 ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/fontbox-2.0.11.jar
 ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/commons-logging-1.2.jar
 ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/jaxb-runtime-2.4.0-b180830.0438.jar
 ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/jaxb-xjc-2.4.0-b180830.0438.jar
 ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/jaxb-jxc-2.4.0-b180830.0438.jar
 ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/jaxb-impl-2.4.0-b180830.0438.jar
 ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/jaxb-api-2.4.0-b180725.0427.jar
 ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/jaxb-core-2.3.0.1.jar
 ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/javax.activation-1.2.0.jar
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/bsaf-1.9.2.jar
  COMMAND curl --retry 4 -O -s -S http://repo.maven.apache.org/maven2/org/jdesktop/bsaf/bsaf/1.9.2/bsaf-1.9.2.jar
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/jcommon-1.0.23.jar
  COMMAND curl --retry 4 -O -s -S http://repo.maven.apache.org/maven2/org/jfree/jcommon/1.0.23/jcommon-1.0.23.jar
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/jfreechart-1.0.19.jar
  COMMAND curl --retry 4 -O -s -S http://repo.maven.apache.org/maven2/org/jfree/jfreechart/1.0.19/jfreechart-1.0.19.jar
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/jfreesvg-2.1.jar
  COMMAND curl --retry 4 -O -s -S http://repo.maven.apache.org/maven2/org/jfree/jfreesvg/2.1/jfreesvg-2.1.jar
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/jopt-simple-4.8.jar
  COMMAND curl --retry 4 -O -s -S http://repo.maven.apache.org/maven2/net/sf/jopt-simple/jopt-simple/4.8/jopt-simple-4.8.jar
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/junit-4.12.jar
  COMMAND curl --retry 4 -O -s -S http://repo1.maven.org/maven2/junit/junit/4.12/junit-4.12.jar
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/swingx-1.6.1.jar
  COMMAND curl --retry 4 -O -s -S http://repo.maven.apache.org/maven2/org/swinglabs/swingx/1.6.1/swingx-1.6.1.jar
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/hamcrest-core-1.3.jar
  COMMAND curl --retry 4 -O -s -S http://repo.maven.apache.org/maven2/org/hamcrest/hamcrest-core/1.3/hamcrest-core-1.3.jar
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/pdfbox-2.0.11.jar
  COMMAND curl --retry 4 -O -s -S http://repo.maven.apache.org/maven2/org/apache/pdfbox/pdfbox/2.0.11/pdfbox-2.0.11.jar
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/fontbox-2.0.11.jar
  COMMAND curl --retry 4 -O -s -S http://repo.maven.apache.org/maven2/org/apache/pdfbox/fontbox/2.0.11/fontbox-2.0.11.jar
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/commons-logging-1.2.jar
  COMMAND curl --retry 4 -O -s -S https://repo1.maven.org/maven2/commons-logging/commons-logging/1.2/commons-logging-1.2.jar
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/jaxb-runtime-2.4.0-b180830.0438.jar
  COMMAND curl --retry 4 -O -s -S http://repo.maven.apache.org/maven2/org/glassfish/jaxb/jaxb-runtime/2.4.0-b180830.0438/jaxb-runtime-2.4.0-b180830.0438.jar
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/jaxb-xjc-2.4.0-b180830.0438.jar
  COMMAND curl --retry 4 -O -s -S http://repo.maven.apache.org/maven2/com/sun/xml/bind/jaxb-xjc/2.4.0-b180830.0438/jaxb-xjc-2.4.0-b180830.0438.jar
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/jaxb-jxc-2.4.0-b180830.0438.jar
  COMMAND curl --retry 4 -O -s -S http://repo.maven.apache.org/maven2/com/sun/xml/bind/jaxb-jxc/2.4.0-b180830.0438/jaxb-jxc-2.4.0-b180830.0438.jar
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/jaxb-impl-2.4.0-b180830.0438.jar
  COMMAND curl --retry 4 -O -s -S http://repo.maven.apache.org/maven2/com/sun/xml/bind/jaxb-impl/2.4.0-b180830.0438/jaxb-impl-2.4.0-b180830.0438.jar
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/jaxb-api-2.4.0-b180725.0427.jar
  COMMAND curl --retry 4 -O -s -S http://repo.maven.apache.org/maven2/com/sun/xml/bind/jaxb-core/2.3.0.1/jaxb-core-2.3.0.1.jar
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/jaxb-core-2.3.0.1.jar
  COMMAND curl --retry 4 -O -s -S http://repo.maven.apache.org/maven2/javax/xml/bind/jaxb-api/2.4.0-b180725.0427/jaxb-api-2.4.0-b180725.0427.jar
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib/javax.activation-1.2.0.jar
  COMMAND curl --retry 4 -O -s -S http://repo.maven.apache.org/maven2/com/sun/activation/javax.activation/1.2.0/javax.activation-1.2.0.jar
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/lib
)

set( JAVA_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/ui/components/CommonTreeNode.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/ui/components/DoubleJSlider.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/ui/components/NumberTextField.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/ui/components/FontChooser.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/ui/panels/AnimationPlayer.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/ui/panels/ConnectionStatusBar.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/ui/panels/DataPanel.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/ui/panels/DynamicTree.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/ui/panels/FindBar.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/ui/panels/ListPanel.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/ui/panels/SmallTrickIconLabel.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/ui/TrickFileFilter.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/ui/UIUtils.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/vs/VSBoolean.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/vs/VSByte.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/vs/VSDouble.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/vs/VSFloat.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/vs/VSInteger.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/vs/VSLong.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/vs/VSShort.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/vs/VSString.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/vs/VSValue.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/vs/Variable.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/vs/VariableListener.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/vs/VariableServerFluent.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/vs/VariableTracker.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/BinaryDataReader.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/CSVDataReader.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/ErrorChecker.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/LogHeaderReader.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/LogVar.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/SortedListModel.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/TrickColors.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/UnitInfixExpression.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/UnitType.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/VariableServerConnection.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/XMLCreator.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/utils/DataReader.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/RunTimeTrickApplication.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/common/TrickApplication.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/plot/utils/editor/DefaultLogAxisEditor.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/plot/utils/editor/TrickAxisEditor.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/plot/utils/editor/TrickChartEditor.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/plot/utils/editor/TrickChartEditorFactory.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/plot/utils/editor/TrickChartEditorManager.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/plot/utils/editor/TrickNumberAxisEditor.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/plot/utils/editor/TrickPlotEditor.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/plot/utils/editor/TrickPolarPlotEditor.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/plot/utils/editor/TrickTitleEditor.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/plot/utils/editor/TrickValueAxisEditor.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/plot/utils/PlotUtils.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/plot/utils/TrickChart.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/plot/utils/TrickChartControlPanel.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/plot/utils/TrickChartPanel.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/plot/utils/TrickChartTheme.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/plot/utils/TrickFrame.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/plot/utils/TrickTableFrame.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/plot/utils/TrickXYLineAndShapeRenderer.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/plot/utils/TrickXYSeries.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/plot/utils/TrickChartFrame.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/plot/utils/TrickXYPlot.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/plot/JXPlotApplication.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickdp/utils/DPRemoteCallInterface.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickdp/utils/DPRemoteCallInterfaceImpl.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickdp/utils/PDFBooklet.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickdp/utils/TrickDPActionController.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickdp/TrickDPApplication.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickqp/utils/DataTransferHandler.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickqp/utils/Product.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickqp/utils/ProductAxis.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickqp/utils/ProductCurve.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickqp/utils/ProductDataPanel.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickqp/utils/ProductDomParser.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickqp/utils/ProductExternalFunction.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickqp/utils/ProductTable.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickqp/utils/ProductTree.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickqp/utils/ProductVarcase.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickqp/utils/QPRemoteCallInterface.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickqp/utils/QPRemoteCallInterfaceImpl.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickqp/utils/VarListPanel.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickqp/utils/CommonProduct.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickqp/utils/ProductPage.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickqp/utils/ProductPlot.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickqp/utils/ProductColumn.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickqp/utils/ProductMeasurement.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickqp/utils/ProductVar.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickqp/utils/ProductXMLCreator.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickqp/utils/TrickQPActionController.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/trickqp/TrickQPApplication.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/utils/FileTreeNode.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/utils/FileTreePanel.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/utils/Session.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/utils/SessionDomParser.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/utils/SessionRun.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/utils/SessionRunTransferHandler.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/utils/SessionXMLCreator.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/utils/SimDPTree.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/utils/SimRunDPTree.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/utils/SimRunTree.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dataproducts/DataProductsApplication.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/dre/DreApplication.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/montemonitor/MonteMonitorApplication.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/montemonitor/Slave.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/mtv/MtvApp.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/mtv/MtvView.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/sie/utils/SearchListener.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/sie/utils/SearchPanel.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/sie/utils/Searcher.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/sie/utils/SieResourceDomParser.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/sie/utils/SieTemplate.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/sie/utils/SieTree.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/sie/utils/SieTreeModel.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/sie/utils/SieVariableTree.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/sie/utils/TreeModelExclusionFilter.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/sie/utils/TreeModelFilter.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/sie/utils/TreeModelSortingFilter.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/sie/utils/VariableList.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/sie/utils/SieEnumeration.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/sie/SieApplication.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/simcontrol/utils/SimControlActionController.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/simcontrol/utils/SimState.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/simcontrol/SimControlApplication.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/sniffer/SimSnifferApplication.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/sniffer/SimulationInformation.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/sniffer/SimulationListener.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/sniffer/SimulationSniffer.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/test/Client.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/tv/StripChart.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/tv/TVApplication.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/tv/TVBean.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/tv/TVBoolean.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/tv/TVDouble.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/tv/TVEnumeration.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/tv/TVFloat.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/tv/TVString.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/tv/TVVariableTree.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/tv/TrickViewFluent.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/tv/VariableTable.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/tv/DoubleComboBox.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/tv/StripChartManager.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/tv/TVByte.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/tv/TVLong.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/tv/TVShort.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/tv/TVInteger.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/vc/VariableCounter.java
  ${CMAKE_CURRENT_SOURCE_DIR}/trick_source/java/src/trick/Template.java
)

add_jar( trick_jar
  SOURCES ${JAVA_SRC}
  INCLUDE_JARS ${JAVA_LIBS}
  OUTPUT_NAME trick
  OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/libexec/trick/java/dist
)
