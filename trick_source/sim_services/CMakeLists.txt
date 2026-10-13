# libtrick.a contains the runtime services and the utility object libraries.
# Keep this inventory aligned with SIM_SERV_DIRS in the top-level Makefile:
# MemoryManager belongs in libtrick_mm.a and InputProcessor in libtrick_pyip.a.
trick_add_library(trick STATIC
    EXPORT_NAME runtime
    SOURCES
        ExternalApplications/ExternalApplication.cpp
        ExternalApplications/ExternalApplicationManager.cpp
        ExternalApplications/ExternalApplication_c_intf.cpp
        ExternalApplications/MalfunctionsTrickView.cpp
        ExternalApplications/MonteMonitor.cpp
        ExternalApplications/SimControlPanel.cpp
        ExternalApplications/StripChart.cpp
        ExternalApplications/TrickView.cpp
        Clock/Clock.cpp
        Clock/GetTimeOfDayClock.cpp
        Clock/clock_c_intf.cpp
        CheckPointAgent/CheckPointAgent.cpp
        CheckPointAgent/ChkPtParseContext.cpp
        CheckPointAgent/ClassicCheckPointAgent.cpp
        CheckPointAgent/PythonPrint.cpp
        CheckPointRestart/CheckPointRestart.cpp
        CheckPointRestart/CheckPointRestart_c_intf.cpp
        CheckPointRestart/next_attr_name.cpp
        CheckPointRestart/stl_type_name_convert.cpp
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
        FrameLog/FrameDataRecordGroup.cpp
        FrameLog/FrameLog.cpp
        FrameLog/FrameLog_c_intf.cpp
        JITInputFile/JITEvent.cpp
        JITInputFile/JITInputFile.cpp
        JITInputFile/jit_input_file_c_intf.cpp
        JSONVariableServer/JSONVariableServer.cpp
        JSONVariableServer/JSONVariableServerThread.cpp
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
        UnitTest/UnitTest.cpp
        UnitTest/UnitTest_c_intf.cpp
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

trick_add_library(trick_mm STATIC
    EXPORT_NAME memory_manager
    SOURCES
        MemoryManager/ADefParseContext.cpp
        MemoryManager/AttributesUtils.cpp
        MemoryManager/MemoryManager.cpp
        MemoryManager/MemoryManager_C_Intf.cpp
        MemoryManager/MemoryManager_JSON_Intf.cpp
        MemoryManager/MemoryManager_add_attr_info.cpp
        MemoryManager/MemoryManager_add_checkpoint_alloc_dependency.cpp
        MemoryManager/MemoryManager_add_shared_library_symbols.cpp
        MemoryManager/MemoryManager_add_template_name_trans.cpp
        MemoryManager/MemoryManager_add_var.cpp
        MemoryManager/MemoryManager_alloc_depends.cpp
        MemoryManager/MemoryManager_alloc_info_map.cpp
        MemoryManager/MemoryManager_clear_memory.cpp
        MemoryManager/MemoryManager_declare_var.cpp
        MemoryManager/MemoryManager_delete_var.cpp
        MemoryManager/MemoryManager_get_attributes_for_address.cpp
        MemoryManager/MemoryManager_get_enumerated.cpp
        MemoryManager/MemoryManager_get_size.cpp
        MemoryManager/MemoryManager_get_stl_dependencies.cpp
        MemoryManager/MemoryManager_get_type_attributes.cpp
        MemoryManager/MemoryManager_io_src_intf.cpp
        MemoryManager/MemoryManager_is_alloced.cpp
        MemoryManager/MemoryManager_make_declaration.cpp
        MemoryManager/MemoryManager_make_reference_attr.cpp
        MemoryManager/MemoryManager_map_external_object.cpp
        MemoryManager/MemoryManager_realloc.cpp
        MemoryManager/MemoryManager_ref_allocate.cpp
        MemoryManager/MemoryManager_ref_assignment.cpp
        MemoryManager/MemoryManager_ref_attributes.cpp
        MemoryManager/MemoryManager_ref_dim.cpp
        MemoryManager/MemoryManager_ref_name.cpp
        MemoryManager/MemoryManager_ref_name_from_address.cpp
        MemoryManager/MemoryManager_ref_var.cpp
        MemoryManager/MemoryManager_restore.cpp
        MemoryManager/MemoryManager_restore_stls.cpp
        MemoryManager/MemoryManager_set_checkpointagent.cpp
        MemoryManager/MemoryManager_set_debug_level.cpp
        MemoryManager/MemoryManager_strdup.cpp
        MemoryManager/MemoryManager_write_checkpoint.cpp
        MemoryManager/MemoryManager_write_var.cpp
        MemoryManager/RefParseContext.cpp
        MemoryManager/ReferenceUtils.cpp
        MemoryManager/addr_bitfield.c
        MemoryManager/extract_bitfield.c
        MemoryManager/extract_unsigned_bitfield.c
        MemoryManager/follow_address_path.c
        MemoryManager/insert_bitfield.c
        MemoryManager/ref_free.cpp
        MemoryManager/ref_to_value.c
        MemoryManager/trickTypeCharString.c
        MemoryManager/vval.c
        MemoryManager/wcs_ext.c
)

# The scanners include the corresponding .tab.hpp. Generate both files in a
# parser-specific binary directory and compile the scanner as C++, like Make.
function(trick_add_parser target directory name)
    set(output_dir "${CMAKE_CURRENT_BINARY_DIR}/${directory}")
    file(MAKE_DIRECTORY "${output_dir}")
    bison_target(${name}
        "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/${name}.y"
        "${output_dir}/${name}.tab.cpp"
        DEFINES_FILE "${output_dir}/${name}.tab.hpp")
    flex_target(${name}
        "${CMAKE_CURRENT_SOURCE_DIR}/${directory}/${name}.l"
        "${output_dir}/${name}.lex.cpp")
    add_flex_bison_dependency(${name} ${name})
    target_sources(${target} PRIVATE
        ${BISON_${name}_OUTPUTS} ${FLEX_${name}_OUTPUTS})
    target_include_directories(${target} PRIVATE "${output_dir}")
endfunction()

trick_add_parser(trick CheckPointAgent input_parser)
trick_add_parser(trick_mm MemoryManager adef_parser)
trick_add_parser(trick_mm MemoryManager ref_parser)

# Generated core attributes (including MemoryManager's public types) belong in libtrick,
# not libtrick_mm, matching the legacy ICG directory-to-archive mapping.
trick_add_icg_sources(trick CORE)

if(TRICK_ENABLE_ER7_UTILS)
    trick_add_icg_sources(er7_utils ER7)
    target_link_libraries(trick PUBLIC er7_utils)
else()
    target_sources(trick PRIVATE
        Integrator/trick_algorithms/ABM_Integrator.cpp
        Integrator/trick_algorithms/Euler_Cromer_Integrator.cpp
        Integrator/trick_algorithms/Euler_Integrator.cpp
        Integrator/trick_algorithms/MM4_Integrator.cpp
        Integrator/trick_algorithms/NL2_Integrator.cpp
        Integrator/trick_algorithms/RK2_Integrator.cpp
        Integrator/trick_algorithms/RK4_Integrator.cpp
        Integrator/trick_algorithms/RKF45_Integrator.cpp
        Integrator/trick_algorithms/RKF78_Integrator.cpp
        Integrator/trick_algorithms/RKG4_Integrator.cpp)
endif()

# Linking OBJECT libraries directly places their objects in libtrick.a and
# carries their usage requirements, unlike a dependency-only edge.
target_link_libraries(trick PRIVATE
    trick_compare_floating_point
    trick_interpolator
    trick_shm
    trick_adt
    trick_unicode)
target_link_libraries(trick PUBLIC
    trick_mm
    trick_comm
    trick_connection_handlers
    trick_math
    trick_units
    UDUNITS2::UDUNITS2
    Threads::Threads
    ${CMAKE_DL_LIBS})
target_link_libraries(trick_mm PUBLIC
    UDUNITS2::UDUNITS2 Threads::Threads ${CMAKE_DL_LIBS})
# MemoryManager needs libtrick's checkpoint/message services at final link,
# not its compile settings or generation dependencies.
target_link_libraries(trick_mm INTERFACE "$<LINK_ONLY:trick>")

if(TRICK_ENABLE_HDF5)
    # HDF5 changes DRHDF5's public layout, not merely its implementation.
    target_compile_definitions(trick PUBLIC HDF5)
    # HL precedes core for static linking; the aggregate target supplies core's
    # external dependencies even with FindHDF5's compiler-wrapper discovery.
    target_link_libraries(trick PUBLIC hdf5::hdf5_hl HDF5::HDF5)
endif()
if(TRICK_ENABLE_GSL)
    target_link_libraries(trick PUBLIC GSL::gsl)
endif()
