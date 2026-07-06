# Sim services C/C++ sources for libtrick.a (top Makefile's SIM_SERV_DIRS
# minus MemoryManager (own archive, liber7_utils split) and InputProcessor
# (goes into libtrick_pyip.a instead) — see A3. List derived by cross
# referencing every non-io_/class_map member of a real `make` build's
# lib/libtrick.a against the source tree (verified 2026-07-06); do not trust
# the 2019 stale CMakeLists.txt list, it predates several renames/additions
# (VariableServerSessionThread -> VariableServerSession, MonteCarloGeneration,
# JSONVariableServerThread, MessageCustomManager, ThreadBase/SysThread).
set( SS_SRC
  CheckPointAgent/CheckPointAgent.cpp
  CheckPointAgent/ChkPtParseContext.cpp
  CheckPointAgent/ClassicCheckPointAgent.cpp
  CheckPointAgent/PythonPrint.cpp
  CheckPointRestart/CheckPointRestart.cpp
  CheckPointRestart/CheckPointRestart_c_intf.cpp
  CheckPointRestart/next_attr_name.cpp
  CheckPointRestart/stl_type_name_convert.cpp
  Clock/Clock.cpp
  Clock/GetTimeOfDayClock.cpp
  Clock/clock_c_intf.cpp
  Collect/collect.cpp
  CommandLineArguments/CommandLineArguments.cpp
  CommandLineArguments/command_line_c_intf.cpp
  DataRecord/DRAscii.cpp
  DataRecord/DRBinary.cpp
  DataRecord/DRHDF5.cpp
  DataRecord/DataRecordDispatcher.cpp
  DataRecord/DataRecordGroup.cpp
  DataRecord/data_record_utilities.cpp
  DebugPause/DebugPause.cpp
  DebugPause/DebugPause_c_intf.cpp
  EchoJobs/EchoJobs.cpp
  EchoJobs/EchoJobs_c_intf.cpp
  Environment/Environment.cpp
  Environment/Environment_c_intf.cpp
  EventManager/EventInstrument.cpp
  EventManager/EventManager.cpp
  EventManager/EventManager_c_intf.cpp
  EventManager/EventProcessor.cpp
  Executive/Executive.cpp
  Executive/ExecutiveException.cpp
  Executive/Executive_add_depends_on_job.cpp
  Executive/Executive_add_jobs_to_queue.cpp
  Executive/Executive_add_scheduled_job_class.cpp
  Executive/Executive_add_sim_object.cpp
  Executive/Executive_advance_sim_time.cpp
  Executive/Executive_c_intf.cpp
  Executive/Executive_call_default_data.cpp
  Executive/Executive_call_initialization.cpp
  Executive/Executive_call_input_processor.cpp
  Executive/Executive_check_all_job_cycle_times.cpp
  Executive/Executive_check_all_jobs_handled.cpp
  Executive/Executive_checkpoint.cpp
  Executive/Executive_clear_scheduled_queues.cpp
  Executive/Executive_create_threads.cpp
  Executive/Executive_fpe_handler.cpp
  Executive/Executive_freeze.cpp
  Executive/Executive_freeze_loop.cpp
  Executive/Executive_get_curr_job.cpp
  Executive/Executive_get_job.cpp
  Executive/Executive_get_job_cycle.cpp
  Executive/Executive_get_process_id.cpp
  Executive/Executive_get_sim_time.cpp
  Executive/Executive_init.cpp
  Executive/Executive_init_freeze_scheduled.cpp
  Executive/Executive_init_signal_handlers.cpp
  Executive/Executive_instrument_job.cpp
  Executive/Executive_isThreadReadyToRun.cpp
  Executive/Executive_loop.cpp
  Executive/Executive_loop_multi_thread.cpp
  Executive/Executive_loop_single_thread.cpp
  Executive/Executive_post_checkpoint.cpp
  Executive/Executive_process_sim_args.cpp
  Executive/Executive_register_scheduler.cpp
  Executive/Executive_remove_jobs.cpp
  Executive/Executive_remove_sim_object.cpp
  Executive/Executive_restart.cpp
  Executive/Executive_run.cpp
  Executive/Executive_scheduled_thread_sync.cpp
  Executive/Executive_set_job_cycle.cpp
  Executive/Executive_set_job_onoff.cpp
  Executive/Executive_set_simobject_onoff.cpp
  Executive/Executive_set_thread_amf_cycle_time.cpp
  Executive/Executive_set_thread_async_wait.cpp
  Executive/Executive_set_thread_cpu_affinity.cpp
  Executive/Executive_set_thread_enabled.cpp
  Executive/Executive_set_thread_priority.cpp
  Executive/Executive_set_thread_process_type.cpp
  Executive/Executive_set_thread_rt_semaphore.cpp
  Executive/Executive_set_time_tic_value.cpp
  Executive/Executive_shutdown.cpp
  Executive/Executive_signal_handler.cpp
  Executive/Executive_stop.cpp
  Executive/Executive_terminate.cpp
  Executive/Executive_thread_sync.cpp
  Executive/Executive_write_s_job_execution.cpp
  Executive/Executive_write_s_run_summary.cpp
  Executive/ThreadTrigger.cpp
  Executive/Threads.cpp
  Executive/Threads_child.cpp
  Executive/Threads_set_amf_cycle_tics.cpp
  Executive/Threads_set_async_wait.cpp
  Executive/Threads_set_process_type.cpp
  Executive/child_handler.cpp
  Executive/fpe_handler.cpp
  Executive/sig_hand.cpp
  ExternalApplications/ExternalApplication.cpp
  ExternalApplications/ExternalApplicationManager.cpp
  ExternalApplications/ExternalApplication_c_intf.cpp
  ExternalApplications/MalfunctionsTrickView.cpp
  ExternalApplications/MonteMonitor.cpp
  ExternalApplications/SimControlPanel.cpp
  ExternalApplications/StripChart.cpp
  ExternalApplications/TrickView.cpp
  FrameLog/FrameDataRecordGroup.cpp
  FrameLog/FrameLog.cpp
  FrameLog/FrameLog_c_intf.cpp
  Integrator/src/IntegLoopManager.cpp
  Integrator/src/IntegLoopScheduler.cpp
  Integrator/src/IntegLoopSimObject.cpp
  Integrator/src/Integrator.cpp
  Integrator/src/Integrator_C_Intf.cpp
  Integrator/src/MultiDtIntegLoopScheduler.cpp
  Integrator/src/MultiDtIntegLoopSimObject.cpp
  Integrator/src/getIntegrator.cpp
  Integrator/src/regula_falsi.c
  Integrator/src/reset_regula_falsi.c
  JITInputFile/JITEvent.cpp
  JITInputFile/JITInputFile.cpp
  JITInputFile/jit_input_file_c_intf.cpp
  JSONVariableServer/JSONVariableServer.cpp
  JSONVariableServer/JSONVariableServerThread.cpp
  MasterSlave/MSSharedMem.cpp
  MasterSlave/MSSocket.cpp
  MasterSlave/Master.cpp
  MasterSlave/Slave.cpp
  Message/MessageCout.cpp
  Message/MessageCustomFile.cpp
  Message/MessageCustomManager.cpp
  Message/MessageFile.cpp
  Message/MessageHSFile.cpp
  Message/MessageLCout.cpp
  Message/MessagePublisher.cpp
  Message/MessageSubscriber.cpp
  Message/MessageTCDevice.cpp
  Message/MessageThreadedCout.cpp
  Message/Message_c_intf.cpp
  Message/PlaybackFile.cpp
  Message/message_publish_standalone.cpp
  MonteCarlo/MonteCarlo.cpp
  MonteCarlo/MonteCarlo_c_intf.cpp
  MonteCarlo/MonteCarlo_dispatch_run_to_slave.cpp
  MonteCarlo/MonteCarlo_dryrun.cpp
  MonteCarlo/MonteCarlo_execute_monte.cpp
  MonteCarlo/MonteCarlo_funcs.cpp
  MonteCarlo/MonteCarlo_initialize_sockets.cpp
  MonteCarlo/MonteCarlo_master.cpp
  MonteCarlo/MonteCarlo_master_file_io.cpp
  MonteCarlo/MonteCarlo_master_init.cpp
  MonteCarlo/MonteCarlo_master_shutdown.cpp
  MonteCarlo/MonteCarlo_receive_results.cpp
  MonteCarlo/MonteCarlo_run_queue.cpp
  MonteCarlo/MonteCarlo_slave.cpp
  MonteCarlo/MonteCarlo_slave_funcs.cpp
  MonteCarlo/MonteCarlo_slave_init.cpp
  MonteCarlo/MonteCarlo_slave_process_run.cpp
  MonteCarlo/MonteCarlo_spawn_slaves.cpp
  MonteCarlo/MonteVarCalculated.cpp
  MonteCarlo/MonteVarFile.cpp
  MonteCarlo/MonteVarFixed.cpp
  MonteCarlo/MonteVarRandom.cpp
  MonteCarlo/StlRandomGenerator.cpp
  MonteCarloGeneration/mc_master.cc
  MonteCarloGeneration/mc_variable.cc
  MonteCarloGeneration/mc_variable_file.cc
  MonteCarloGeneration/mc_variable_random_normal.cc
  MonteCarloGeneration/mc_variable_random_string.cc
  MonteCarloGeneration/mc_variable_random_uniform.cc
  RealtimeInjector/RtiEvent.cpp
  RealtimeInjector/RtiExec.cpp
  RealtimeInjector/RtiList.cpp
  RealtimeInjector/RtiStager.cpp
  RealtimeSync/RealtimeSync.cpp
  RealtimeSync/RealtimeSync_c_intf.cpp
  ScheduledJobQueue/ScheduledJobQueue.cpp
  ScheduledJobQueue/ScheduledJobQueueInstrument.cpp
  Scheduler/Scheduler.cpp
  Sie/AttributesMap.cpp
  Sie/EnumAttributesMap.cpp
  Sie/Sie.cpp
  Sie/sie_c_intf.cpp
  SimObject/JobData.cpp
  SimObject/SimObject.cpp
  SimTime/SimTime.cpp
  SimTime/SimTime_c_intf.cpp
  ThreadBase/SysThread.cpp
  ThreadBase/ThreadBase.cpp
  Timer/ITimer.cpp
  Timer/Timer.cpp
  Timer/it_handler.cpp
  UdUnits/UdUnits.cpp
  UdUnits/map_trick_units_to_udunits.cpp
  UnitTest/UnitTest.cpp
  UnitTest/UnitTest_c_intf.cpp
  UnitsMap/UnitsMap.cpp
  VariableServer/VariableReference.cpp
  VariableServer/VariableServer.cpp
  VariableServer/VariableServerListenThread.cpp
  VariableServer/VariableServerSession.cpp
  VariableServer/VariableServerSessionThread.cpp
  VariableServer/VariableServerSessionThread_loop.cpp
  VariableServer/VariableServerSession_commands.cpp
  VariableServer/VariableServerSession_copy_and_write_modes.cpp
  VariableServer/VariableServerSession_copy_sim_data.cpp
  VariableServer/VariableServerSession_freeze_init.cpp
  VariableServer/VariableServerSession_write_data.cpp
  VariableServer/VariableServerSession_write_stdio.cpp
  VariableServer/VariableServer_copy_and_write_freeze.cpp
  VariableServer/VariableServer_copy_and_write_freeze_scheduled.cpp
  VariableServer/VariableServer_copy_and_write_scheduled.cpp
  VariableServer/VariableServer_copy_and_write_top.cpp
  VariableServer/VariableServer_default_data.cpp
  VariableServer/VariableServer_freeze_init.cpp
  VariableServer/VariableServer_get_next_freeze_call_time.cpp
  VariableServer/VariableServer_get_next_sync_call_time.cpp
  VariableServer/VariableServer_init.cpp
  VariableServer/VariableServer_open_additional_servers.cpp
  VariableServer/VariableServer_restart.cpp
  VariableServer/VariableServer_shutdown.cpp
  VariableServer/exit_var_thread.cpp
  VariableServer/var_server_ext.cpp
  Zeroconf/Zeroconf.cpp
  mains/master.cpp
)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)
flex_target(trick_ss_input_lexer CheckPointAgent/input_parser.l ${CMAKE_CURRENT_BINARY_DIR}/input_parser.lex.cpp)
bison_target(trick_ss_input_parser CheckPointAgent/input_parser.y ${CMAKE_CURRENT_BINARY_DIR}/input_parser.tab.cpp)
set_source_files_properties(${FLEX_trick_ss_input_lexer_OUTPUTS} PROPERTIES
    COMPILE_OPTIONS "-Wno-unused-parameter;-Wno-unused-function;-Wno-sign-compare;-x;c++"
)
set_source_files_properties(${BISON_trick_ss_input_parser_OUTPUTS} PROPERTIES
    COMPILE_OPTIONS "-Wno-unused-parameter;-Wno-unused;-x;c++"
)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set_property(SOURCE ${BISON_trick_ss_input_parser_OUTPUTS} APPEND PROPERTY
        COMPILE_OPTIONS "-Wno-parentheses-equality")
endif()

add_library( sim_services_objs OBJECT
    ${SS_SRC}
    ${FLEX_trick_ss_input_lexer_OUTPUTS}
    ${BISON_trick_ss_input_parser_OUTPUTS}
)
target_link_libraries( sim_services_objs PUBLIC trick_build_flags )

# HDF5 support in DataRecord/DRHDF5.cpp (Makefile: DataRecord/Makefile:6-9).
if(HDF5_HOME)
    set_property(SOURCE DataRecord/DRHDF5.cpp APPEND PROPERTY COMPILE_DEFINITIONS HDF5)
    if(NOT HDF5_HOME STREQUAL "/usr")
        set_property(SOURCE DataRecord/DRHDF5.cpp APPEND PROPERTY COMPILE_OPTIONS "-I${HDF5_HOME}/include")
    endif()
endif()
# GSL support for MonteCarlo.cpp comes from trick_build_flags (root
# CMakeLists.txt), not a per-file override here: MonteCarlo/Makefile's own
# GSL block is gated on $(HAVE_GSL), a variable nothing in the config system
# ever sets — dead code. The _HAVE_GSL define MonteCarlo.cpp (and
# include/trick/rand_generator.h) actually see in the real build comes
# entirely from Makefile.common's global TRICK_SYSTEM_CXXFLAGS.

add_subdirectory(MemoryManager)
add_subdirectory(InputProcessor)
