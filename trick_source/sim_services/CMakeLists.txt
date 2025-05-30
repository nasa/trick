
# Sim services C/C++ files
set( SS_SRC
  CheckPointAgent/CheckPointAgent
  CheckPointAgent/ChkPtParseContext
  CheckPointAgent/ClassicCheckPointerAgent
  CheckPointAgent/PythonPrint
  CheckPointRestart/CheckPointRestart
  CheckPointRestart/CheckPointRestart_c_intf
  CheckPointRestart/next_attr_name
  CheckPointRestart/stl_type_name_convert
  Clock/BC635Clock
  Clock/Clock
  Clock/GetTimeOfDayClock
  Clock/TPROCTEClock
  Clock/clock_c_intf
  Collect/collect
  CommandLineArguments/CommandLineArguments
  CommandLineArguments/command_line_c_intf
  DataRecord/DRAscii
  DataRecord/DRBinary
  DataRecord/DRHDF5
  DataRecord/DataRecordDispatcher
  DataRecord/DataRecordGroup
  DataRecord/data_record_utilities
  DebugPause/DebugPause
  DebugPause/DebugPause_c_intf
  EchoJobs/EchoJobs
  EchoJobs/EchoJobs_c_intf
  Environment/Environment
  Environment/Environment_c_intf
  EventManager/EventInstrument
  EventManager/EventManager
  EventManager/EventManager_c_intf
  EventManager/EventProcessor
  Executive/Executive
  Executive/ExecutiveException
  Executive/Executive_add_depends_on_job
  Executive/Executive_add_jobs_to_queue
  Executive/Executive_add_scheduled_job_class
  Executive/Executive_add_sim_object
  Executive/Executive_advance_sim_time
  Executive/Executive_c_intf
  Executive/Executive_call_default_data
  Executive/Executive_call_initialization
  Executive/Executive_call_input_processor
  Executive/Executive_check_all_job_cycle_times
  Executive/Executive_check_all_jobs_handled
  Executive/Executive_checkpoint
  Executive/Executive_clear_scheduled_queues
  Executive/Executive_create_threads
  Executive/Executive_fpe_handler
  Executive/Executive_freeze
  Executive/Executive_freeze_loop
  Executive/Executive_get_curr_job
  Executive/Executive_get_job
  Executive/Executive_get_job_cycle
  Executive/Executive_get_process_id
  Executive/Executive_get_sim_time
  Executive/Executive_init
  Executive/Executive_init_freeze_scheduled
  Executive/Executive_init_signal_handlers
  Executive/Executive_instrument_job
  Executive/Executive_isThreadReadyToRun
  Executive/Executive_loop
  Executive/Executive_loop_multi_thread
  Executive/Executive_loop_single_thread
  Executive/Executive_post_checkpoint
  Executive/Executive_process_sim_args
  Executive/Executive_register_scheduler
  Executive/Executive_remove_jobs
  Executive/Executive_remove_sim_object
  Executive/Executive_restart
  Executive/Executive_run
  Executive/Executive_scheduled_thread_sync
  Executive/Executive_set_job_cycle
  Executive/Executive_set_job_onoff
  Executive/Executive_set_simobject_onoff
  Executive/Executive_set_thread_amf_cycle_time
  Executive/Executive_set_thread_async_wait
  Executive/Executive_set_thread_cpu_affinity
  Executive/Executive_set_thread_enabled
  Executive/Executive_set_thread_priority
  Executive/Executive_set_thread_process_type
  Executive/Executive_set_thread_rt_semaphore
  Executive/Executive_set_time_tic_value
  Executive/Executive_shutdown
  Executive/Executive_signal_handler
  Executive/Executive_stop
  Executive/Executive_terminate
  Executive/Executive_thread_sync
  Executive/Executive_write_s_job_execution
  Executive/Executive_write_s_run_summary
  Executive/ThreadTrigger
  Executive/Threads
  Executive/Threads_child
  Executive/Threads_set_amf_cycle_tics
  Executive/Threads_set_async_wait
  Executive/Threads_set_process_type
  Executive/child_handler
  Executive/fpe_handler
  Executive/sig_hand
  ExternalApplications/ExternalApplication
  ExternalApplications/ExternalApplicationManager
  ExternalApplications/ExternalApplication_c_intf
  ExternalApplications/MalfunctionsTrickView
  ExternalApplications/MonteMonitor
  ExternalApplications/SimControlPanel
  ExternalApplications/StripChart
  ExternalApplications/TrickView
  FrameLog/FrameDataRecordGroup
  FrameLog/FrameLog
  FrameLog/FrameLog_c_intf
  Integrator/src/IntegLoopManager
  Integrator/src/IntegLoopScheduler
  Integrator/src/IntegLoopSimObject
  Integrator/src/MultiDtIntegLoopScheduler
  Integrator/src/MultiDtIntegLoopSimObject
  Integrator/src/Integrator
  Integrator/src/Integrator_C_Intf
  Integrator/src/getIntegrator
  Integrator/src/regula_falsi
  Integrator/src/reset_regula_falsi
  JITInputFile/JITEvent
  JITInputFile/JITInputFile
  JITInputFile/jit_input_file_c_intf
  JSONVariableServer/JSONVariableServer
  JSONVariableServer/JSONVariableServerSessionThread
  MasterSlave/MSSharedMem
  MasterSlave/MSSocket
  MasterSlave/Master
  MasterSlave/Slave
  Message/MessageCout
  Message/MessageFile
  Message/MessageHSFile
  Message/MessageCustomFile
  Message/MessageLCout
  Message/MessagePublisher
  Message/MessageSubscriber
  Message/MessageTCDevice
  Message/MessageThreadedCout
  Message/Message_c_intf
  Message/PlaybackFile
  Message/message_publish_standalone
  MonteCarlo/MonteCarlo
  MonteCarlo/MonteCarlo_c_intf
  MonteCarlo/MonteCarlo_dispatch_run_to_slave
  MonteCarlo/MonteCarlo_dryrun
  MonteCarlo/MonteCarlo_execute_monte
  MonteCarlo/MonteCarlo_funcs
  MonteCarlo/MonteCarlo_initialize_sockets
  MonteCarlo/MonteCarlo_master
  MonteCarlo/MonteCarlo_master_file_io
  MonteCarlo/MonteCarlo_master_init
  MonteCarlo/MonteCarlo_master_shutdown
  MonteCarlo/MonteCarlo_receive_results
  MonteCarlo/MonteCarlo_run_queue
  MonteCarlo/MonteCarlo_slave
  MonteCarlo/MonteCarlo_slave_funcs
  MonteCarlo/MonteCarlo_slave_init
  MonteCarlo/MonteCarlo_slave_process_run
  MonteCarlo/MonteCarlo_spawn_slaves
  MonteCarlo/MonteVarCalculated
  MonteCarlo/MonteVarFile
  MonteCarlo/MonteVarFixed
  MonteCarlo/MonteVarRandom
  MonteCarlo/StlRandomGenerator
  RealtimeInjector/RtiEvent
  RealtimeInjector/RtiExec
  RealtimeInjector/RtiList
  RealtimeInjector/RtiStager
  RealtimeSync/RealtimeSync
  RealtimeSync/RealtimeSync_c_intf
  ScheduledJobQueue/ScheduledJobQueue
  ScheduledJobQueue/ScheduledJobQueueInstrument
  Scheduler/Scheduler
  Sie/AttributesMap
  Sie/EnumAttributesMap
  Sie/Sie
  Sie/sie_c_intf
  SimObject/JobData
  SimObject/SimObject
  SimTime/SimTime
  SimTime/SimTime_c_intf
  ThreadBase/ThreadBase
  Timer/ITimer
  Timer/Timer
  Timer/it_handler
  UdUnits/UdUnits
  UdUnits/map_trick_units_to_udunits
  UnitTest/UnitTest
  UnitTest/UnitTest_c_intf
  UnitsMap/UnitsMap
  VariableServer/VariableReference
  VariableServer/VariableServer
  VariableServer/VariableServerListenThread
  VariableServer/VariableServerSessionThread
  VariableServer/VariableServerSessionThread_commands
  VariableServer/VariableServerSessionThread_connect
  VariableServer/VariableServerSessionThread_copy_data
  VariableServer/VariableServerSessionThread_copy_sim_data
  VariableServer/VariableServerSessionThread_create_socket
  VariableServer/VariableServerSessionThread_freeze_init
  VariableServer/VariableServerSessionThread_loop
  VariableServer/VariableServerSessionThread_restart
  VariableServer/VariableServerSessionThread_write_data
  VariableServer/VariableServerSessionThread_write_stdio
  VariableServer/VariableServer_copy_and_write_freeze
  VariableServer/VariableServer_copy_and_write_freeze_scheduled
  VariableServer/VariableServer_copy_and_write_scheduled
  VariableServer/VariableServer_copy_and_write_top
  VariableServer/VariableServer_default_data
  VariableServer/VariableServer_freeze_init
  VariableServer/VariableServer_get_next_freeze_call_time
  VariableServer/VariableServer_get_next_sync_call_time
  VariableServer/VariableServer_open_additional_servers
  VariableServer/VariableServer_init
  VariableServer/VariableServer_restart
  VariableServer/VariableServer_shutdown
  VariableServer/exit_var_thread
  VariableServer/var_server_ext
  Zeroconf/Zeroconf
  mains/master
)

# Sim services Lex/Yacc files
set( SS_LEX_YACC_SRC
  ${CMAKE_BINARY_DIR}/temp_src/lex_yacc/input_parser.lex
  ${CMAKE_BINARY_DIR}/temp_src/lex_yacc/input_parser.tab
)

add_custom_command( OUTPUT ${CMAKE_BINARY_DIR}/temp_src/lex_yacc/input_parser.lex.cpp
 COMMAND ${FLEX_EXECUTABLE} -d -o ${CMAKE_BINARY_DIR}/temp_src/lex_yacc/input_parser.lex.cpp ${CMAKE_CURRENT_SOURCE_DIR}/CheckPointAgent/input_parser.l
 MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/CheckPointAgent/input_parser.l
)
add_custom_command( OUTPUT ${CMAKE_BINARY_DIR}/temp_src/lex_yacc/input_parser.tab.cpp ${CMAKE_BINARY_DIR}/temp_src/lex_yacc/input_parser.tab.hpp
 COMMAND ${BISON_EXECUTABLE} -d -o ${CMAKE_BINARY_DIR}/temp_src/lex_yacc/input_parser.tab.cpp ${CMAKE_CURRENT_SOURCE_DIR}/CheckPointAgent/input_parser.y
 MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/CheckPointAgent/input_parser.y
)

add_library( sim_services_objs OBJECT ${SS_SRC} ${SS_LEX_YACC_SRC})
target_include_directories( sim_services_objs PUBLIC ${PYTHON_INCLUDE_DIRS} )
target_include_directories( sim_services_objs PUBLIC ${UDUNITS2_INCLUDES} )

add_subdirectory(MemoryManager)
add_subdirectory(InputProcessor)

