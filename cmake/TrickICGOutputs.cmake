# Relative inventory shared by scheduling, validation and publication.
# CMake needs these outputs before ICG runs. Each header with eligible,
# non-excluded classes or enums produces one io_<basename>.cpp; core headers
# share sim_services/io_src, while er7 headers retain their directory layout.
# Update these lists when adding or removing such headers.

set(_icg_core_names
    ABM_Integrator AttributesMap CheckPointAgent CheckPointRestart ClientConnection
    Clock CommandLineArguments DRAscii DRBinary DRHDF5 DataRecordDispatcher
    DataRecordGroup DebugPause EchoJobs EnumAttributesMap Environment
    Euler_Cromer_Integrator Euler_Integrator Event EventInstrument EventManager
    EventProcessor Executive ExecutiveException ExternalApplication Flag
    FrameDataRecordGroup FrameLog GetTimeOfDayClock IPPython IPPythonEvent ITimer
    InputProcessor InstrumentBase IntegLoopManager IntegLoopScheduler
    IntegLoopSimObject Integrator JITEvent JITInputFile JSONVariableServer
    JSONVariableServerThread JobData MM4_Integrator MSConnect MSSharedMem MSSocket
    MTV MalfunctionsTrickView Master MemoryManager MessageCout MessageCustomFile
    MessageCustomManager MessageFile MessageHSFile MessageLCout MessagePublisher
    MessageSubscriber MessageTCDevice MessageThreadedCout MonteCarlo MonteMonitor
    MonteVar MultiDtIntegLoopScheduler MultiDtIntegLoopSimObject MulticastGroup
    NL2_Integrator PlaybackFile RK2_Integrator RK4_Integrator RKF45_Integrator
    RKF78_Integrator RKG4_Integrator RealtimeSync RemoteShell RtiEvent RtiExec
    RtiList RtiStager STLInterface ScheduledJobQueue Scheduler Sie SimControlPanel
    SimObject SimTime Slave StripChart SysThread SystemInterface TCPClientListener
    TCPConnection ThreadBase ThreadTrigger Threads Timer TrickView UCFn UDPConnection
    UdUnits Unit UnitTest UnitsMap VariableReference VariableServer
    VariableServerListenThread VariableServerSession VariableServerSessionThread
    Zeroconf attributes dllist io_alloc lqueue lstack mc_master mc_python_code
    mc_variable mc_variable_file mc_variable_fixed mc_variable_random
    mc_variable_random_bool mc_variable_random_normal mc_variable_random_string
    mc_variable_random_uniform mc_variable_semi_fixed message_type mm_error
    ms_sim_mode parameter_types rand_generator reference reference_frame
    regula_falsi sim_mode tc time_offset trick_error_hndlr tsm units_conv value var
    variable_server_message_types variable_server_sync_types wave_form)
if(TRICK_ENABLE_CIVETWEB)
    list(APPEND _icg_core_names MyCivetServer WebSocketSession)
endif()
set(_icg_core_sources "sim_services/io_src/class_map.cpp")
foreach(_name IN LISTS _icg_core_names)
    list(APPEND _icg_core_sources "sim_services/io_src/io_${_name}.cpp")
endforeach()

set(_icg_er7_sources)
if(TRICK_ENABLE_ER7_UTILS)
    set(_icg_er7_headers
        integration/abm4/abm4_first_order_ode_integrator
        integration/abm4/abm4_integrator_constructor
        integration/abm4/abm4_second_order_ode_integrator
        integration/beeman/beeman_integrator_constructor
        integration/beeman/beeman_second_order_ode_integrator
        integration/core/base_integration_group
        integration/core/bogus_integration_controls
        integration/core/first_order_ode_integrator
        integration/core/generalized_position_derivative
        integration/core/integrable_object
        integration/core/integration_controls
        integration/core/integration_messages
        integration/core/integration_technique
        integration/core/integrator_constructor
        integration/core/integrator_constructor_factory
        integration/core/integrator_interface
        integration/core/integrator_result
        integration/core/integrator_result_merger
        integration/core/integrator_result_merger_container
        integration/core/left_quaternion_functions
        integration/core/priming_first_order_ode_integrator
        integration/core/priming_integration_controls
        integration/core/priming_integrator_constructor
        integration/core/priming_second_order_ode_integrator
        integration/core/second_order_ode_integrator
        integration/core/single_cycle_integration_controls
        integration/core/standard_integration_controls
        integration/core/state_integrator
        integration/core/time_interface
        integration/euler/euler_first_order_ode_integrator
        integration/euler/euler_integrator_constructor
        integration/euler/euler_second_order_ode_integrator
        integration/mm4/mm4_integrator_constructor
        integration/mm4/mm4_second_order_ode_integrator
        integration/nl2/nl2_integrator_constructor
        integration/nl2/nl2_second_order_ode_integrator
        integration/position_verlet/position_verlet_integrator_constructor
        integration/position_verlet/position_verlet_second_order_ode_integrator
        integration/rk2_heun/rk2_heun_first_order_ode_integrator
        integration/rk2_heun/rk2_heun_integrator_constructor
        integration/rk2_heun/rk2_heun_second_order_ode_integrator
        integration/rk2_midpoint/rk2_midpoint_first_order_ode_integrator
        integration/rk2_midpoint/rk2_midpoint_integrator_constructor
        integration/rk2_midpoint/rk2_midpoint_second_order_ode_integrator
        integration/rk4/rk4_first_order_ode_integrator
        integration/rk4/rk4_integrator_constructor
        integration/rk4/rk4_second_order_ode_integrator
        integration/rk4/rk4_second_order_ode_integrator_base
        integration/rkf45/rkf45_first_order_ode_integrator
        integration/rkf45/rkf45_integrator_constructor
        integration/rkf45/rkf45_second_order_ode_integrator
        integration/rkf78/rkf78_first_order_ode_integrator
        integration/rkf78/rkf78_integrator_constructor
        integration/rkf78/rkf78_second_order_ode_integrator
        integration/rkg4/rkg4_first_order_ode_integrator
        integration/rkg4/rkg4_integrator_constructor
        integration/rkg4/rkg4_second_order_ode_integrator
        integration/symplectic_euler/symplectic_euler_integrator_constructor
        integration/symplectic_euler/symplectic_euler_second_order_ode_integrator
        integration/velocity_verlet/velocity_verlet_integrator_constructor
        integration/velocity_verlet/velocity_verlet_second_order_ode_integrator
        interface/deletable
        math/n_choose_m math/ratio128 math/uint128
        trick/integration/trick_first_order_ode_integrator
        trick/integration/trick_integrator
        trick/integration/trick_second_order_ode_integrator)
    foreach(_header IN LISTS _icg_er7_headers)
        get_filename_component(_dir "${_header}" DIRECTORY)
        get_filename_component(_name "${_header}" NAME)
        list(APPEND _icg_er7_sources
            "er7_utils/${_dir}/include/io_src/io_${_name}.cpp")
    endforeach()
endif()

