<?xml version="1.0"?>

<sie>

  <sim_object>trick_sys</sim_object>

  <template>
    <struct_name>trick_sys</struct_name>
    <parameter>sched</parameter>
    <type_name>Trick::MonteCarlo</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>enabled</first_child>
  </template>

  <template>
    <struct_name>trick_sys</struct_name>
    <parameter>env</parameter>
    <type_name>Trick::Environment</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
  </template>

  <sim_object>trick_mm</sim_object>

  <template>
    <struct_name>trick_mm</struct_name>
    <parameter>mmw</parameter>
    <type_name>Trick::MemoryManagerWrapper</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>safestore_period</first_child>
  </template>

  <sim_object>trick_cmd_args</sim_object>

  <template>
    <struct_name>trick_cmd_args</struct_name>
    <parameter>cmd_args</parameter>
    <type_name>Trick::CommandLineArguments</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>output_dir_timestamped</first_child>
  </template>

  <sim_object>message</sim_object>

  <template>
    <struct_name>message</struct_name>
    <parameter>mpublisher</parameter>
    <type_name>Trick::MessagePublisher</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>sim_name</first_child>
  </template>

  <template>
    <struct_name>message</struct_name>
    <parameter>mcout</parameter>
    <type_name>Trick::MessageCout</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>enabled</first_child>
  </template>

  <template>
    <struct_name>message</struct_name>
    <parameter>mfile</parameter>
    <type_name>Trick::MessageFile</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>file_name</first_child>
  </template>

  <template>
    <struct_name>message</struct_name>
    <parameter>mdevice</parameter>
    <type_name>Trick::MessageTCDevice</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>port</first_child>
  </template>

  <sim_object>trick_ip</sim_object>

  <template>
    <struct_name>trick_ip</struct_name>
    <parameter>ip</parameter>
    <type_name>Trick::IPPython</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>mtv_list</first_child>
  </template>

  <sim_object>monte_carlo</sim_object>

  <sim_object>trick_vs</sim_object>

  <template>
    <struct_name>trick_vs</struct_name>
    <parameter>vs</parameter>
    <type_name>Trick::VariableServer</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
  </template>

  <template>
    <struct_name>trick_vs</struct_name>
    <parameter>sim_control_panel</parameter>
    <type_name>Trick::SimControlPanel</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
  </template>

  <template>
    <struct_name>trick_vs</struct_name>
    <parameter>trick_view</parameter>
    <type_name>Trick::TrickView</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
  </template>

  <template>
    <struct_name>trick_vs</struct_name>
    <parameter>malfunctions_trick_view</parameter>
    <type_name>Trick::MalfunctionsTrickView</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
  </template>

  <template>
    <struct_name>trick_vs</struct_name>
    <parameter>monte_monitor</parameter>
    <type_name>Trick::MonteMonitor</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
  </template>

  <template>
    <struct_name>trick_vs</struct_name>
    <parameter>stripchart</parameter>
    <type_name>Trick::StripChart</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
  </template>

  <sim_object>data_record</sim_object>

  <template>
    <struct_name>data_record</struct_name>
    <parameter>drd</parameter>
    <type_name>Trick::DataRecordDispatcher</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
  </template>

  <template>
    <struct_name>data_record</struct_name>
    <parameter>dr_thread</parameter>
    <type_name>Trick::DataRecordThreaded</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>cpu_number</first_child>
  </template>

  <template>
    <struct_name>data_record</struct_name>
    <parameter>dr_disk</parameter>
    <type_name>Trick::DataRecordDisk</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>buffering</first_child>
  </template>

  <template>
    <struct_name>data_record</struct_name>
    <parameter>dr_ring</parameter>
    <type_name>Trick::DataRecordRing</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>buffering</first_child>
  </template>

  <sim_object>master_slave</sim_object>

  <template>
    <struct_name>master_slave</struct_name>
    <parameter>master</parameter>
    <type_name>Trick::Master</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>enabled</first_child>
  </template>

  <template>
    <struct_name>master_slave</struct_name>
    <parameter>slave</parameter>
    <type_name>Trick::Slave</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>enabled</first_child>
  </template>

  <sim_object>real_time</sim_object>

  <template>
    <struct_name>real_time</struct_name>
    <parameter>gtod_clock</parameter>
    <type_name>Trick::GetTimeOfDayClock</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>rt_clock_ratio</first_child>
  </template>

  <template>
    <struct_name>real_time</struct_name>
    <parameter>itimer</parameter>
    <type_name>Trick::ITimer</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>enabled</first_child>
  </template>

  <template>
    <struct_name>real_time</struct_name>
    <parameter>rt_sync</parameter>
    <type_name>Trick::RealtimeSync</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>enable_flag</first_child>
  </template>

  <template>
    <struct_name>real_time</struct_name>
    <parameter>frame_log</parameter>
    <type_name>Trick::FrameLog</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
  </template>

  <sim_object>instruments</sim_object>

  <template>
    <struct_name>instruments</struct_name>
    <parameter>echo_jobs</parameter>
    <type_name>Trick::EchoJobs</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
  </template>

  <template>
    <struct_name>instruments</struct_name>
    <parameter>debug_pause</parameter>
    <type_name>Trick::DebugPause</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>debug_pause_flag</first_child>
  </template>

  <sim_object>dmtcp</sim_object>

  <template>
    <struct_name>dmtcp</struct_name>
    <parameter>dmtcp</parameter>
    <type_name>Trick::DMTCP</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>checkpoint_now</first_child>
  </template>

  <sim_object>zero_conf</sim_object>

  <template>
    <struct_name>zero_conf</struct_name>
    <parameter>zc</parameter>
    <type_name>Trick::Zeroconf</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
  </template>

  <sim_object>utest</sim_object>

  <template>
    <struct_name>utest</struct_name>
    <parameter>unit_tests</parameter>
    <type_name>Trick::UnitTest</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>enabled</first_child>
  </template>

  <sim_object>ball</sim_object>

  <template>
    <struct_name>ball</struct_name>
    <parameter>obj</parameter>
    <type_name>Ball</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>state</first_child>
  </template>

  <template>
    <struct_name>ball</struct_name>
    <parameter>my_integ</parameter>
    <type_name>Trick::Integrator</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>0</dim0>
    <first_child>num_state</first_child>
  </template>

  <sim_object>my_integ_loop</sim_object>

  <template>
    <struct_name>my_integ_loop</struct_name>
    <parameter>integ_sched</parameter>
    <type_name>Trick::IntegLoopScheduler</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>cycle</first_child>
  </template>

  <template>
    <struct_name>Ball</struct_name>
    <parameter>state</parameter>
    <type_name>BallState</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>input</first_child>
    <next_sibling>force</next_sibling>
  </template>

  <template>
    <struct_name>Ball</struct_name>
    <parameter>force</parameter>
    <type_name>BallForce</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>input</first_child>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>BallForce</struct_name>
    <parameter>input</parameter>
    <type_name>BallForceInput</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>origin</first_child>
    <next_sibling>output</next_sibling>
  </template>

  <template>
    <struct_name>BallForce</struct_name>
    <parameter>output</parameter>
    <type_name>BallForceOutput</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>force</first_child>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>BallForceInput</struct_name>
    <parameter>origin</parameter>
    <type_name>double</type_name>
    <io_attributes>InputOnly</io_attributes>
    <units>m</units>
    <type_def>other</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>2</dim0>
    <next_sibling>force</next_sibling>
  </template>

  <template>
    <struct_name>BallForceInput</struct_name>
    <parameter>force</parameter>
    <type_name>double</type_name>
    <io_attributes>InputOnly</io_attributes>
    <units>N</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>BallForceOutput</struct_name>
    <parameter>force</parameter>
    <type_name>double</type_name>
    <io_attributes>OutputOnly</io_attributes>
    <units>N</units>
    <type_def>other</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>2</dim0>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>BallState</struct_name>
    <parameter>input</parameter>
    <type_name>BallStateInput</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>mass</first_child>
    <next_sibling>output</next_sibling>
  </template>

  <template>
    <struct_name>BallState</struct_name>
    <parameter>output</parameter>
    <type_name>BallStateOutput</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>position</first_child>
    <next_sibling>work</next_sibling>
  </template>

  <template>
    <struct_name>BallState</struct_name>
    <parameter>work</parameter>
    <type_name>BallStateWork</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>BallStateInput</struct_name>
    <parameter>mass</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>position</next_sibling>
  </template>

  <template>
    <struct_name>BallStateInput</struct_name>
    <parameter>position</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>2</dim0>
    <next_sibling>speed</next_sibling>
  </template>

  <template>
    <struct_name>BallStateInput</struct_name>
    <parameter>speed</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>elevation</next_sibling>
  </template>

  <template>
    <struct_name>BallStateInput</struct_name>
    <parameter>elevation</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>BallStateOutput</struct_name>
    <parameter>position</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>m</units>
    <type_def>other</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>2</dim0>
    <next_sibling>velocity</next_sibling>
  </template>

  <template>
    <struct_name>BallStateOutput</struct_name>
    <parameter>velocity</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>m/s</units>
    <type_def>other</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>2</dim0>
    <next_sibling>acceleration</next_sibling>
  </template>

  <template>
    <struct_name>BallStateOutput</struct_name>
    <parameter>acceleration</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>m/s2</units>
    <type_def>other</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>2</dim0>
    <next_sibling>external_force</next_sibling>
  </template>

  <template>
    <struct_name>BallStateOutput</struct_name>
    <parameter>external_force</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>N</units>
    <type_def>other</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>2</dim0>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>TCDevice</struct_name>
    <parameter>disabled</parameter>
    <type_name>TCCommFlag</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>enumerated</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>disable_handshaking</next_sibling>
  </template>

  <template>
    <struct_name>TCDevice</struct_name>
    <parameter>disable_handshaking</parameter>
    <type_name>TCCommFlag</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>enumerated</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>dmtcp_use_real</next_sibling>
  </template>

  <template>
    <struct_name>TCDevice</struct_name>
    <parameter>dmtcp_use_real</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>blockio_limit</next_sibling>
  </template>

  <template>
    <struct_name>TCDevice</struct_name>
    <parameter>blockio_limit</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>s</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>blockio_type</next_sibling>
  </template>

  <template>
    <struct_name>TCDevice</struct_name>
    <parameter>blockio_type</parameter>
    <type_name>TCCommBlocking</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>enumerated</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>client_id</next_sibling>
  </template>

  <template>
    <struct_name>TCDevice</struct_name>
    <parameter>client_id</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>client_tag</next_sibling>
  </template>

  <template>
    <struct_name>TCDevice</struct_name>
    <parameter>client_tag</parameter>
    <type_name>char</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>80</dim0>
    <next_sibling>error_handler</next_sibling>
  </template>

  <template>
    <struct_name>TCDevice</struct_name>
    <parameter>error_handler</parameter>
    <type_name>_TrickErrorHndlr</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>0</dim0>
    <first_child>report_level</first_child>
    <next_sibling>hostname</next_sibling>
  </template>

  <template>
    <struct_name>TCDevice</struct_name>
    <parameter>hostname</parameter>
    <type_name>char</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>0</dim0>
    <next_sibling>port</next_sibling>
  </template>

  <template>
    <struct_name>TCDevice</struct_name>
    <parameter>port</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>mcast_group</next_sibling>
  </template>

  <template>
    <struct_name>TCDevice</struct_name>
    <parameter>mcast_group</parameter>
    <type_name>char</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>0</dim0>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::Clock</struct_name>
    <parameter>rt_clock_ratio</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::CommandLineArguments</struct_name>
    <parameter>output_dir_timestamped</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>default_dir</next_sibling>
  </template>

  <template>
    <struct_name>Trick::CommandLineArguments</struct_name>
    <parameter>default_dir</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>cmdline_name</next_sibling>
  </template>

  <template>
    <struct_name>Trick::CommandLineArguments</struct_name>
    <parameter>cmdline_name</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>run_dir</next_sibling>
  </template>

  <template>
    <struct_name>Trick::CommandLineArguments</struct_name>
    <parameter>run_dir</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>user_output_dir</next_sibling>
  </template>

  <template>
    <struct_name>Trick::CommandLineArguments</struct_name>
    <parameter>user_output_dir</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>time_stamp_dir</next_sibling>
  </template>

  <template>
    <struct_name>Trick::CommandLineArguments</struct_name>
    <parameter>time_stamp_dir</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>output_dir</next_sibling>
  </template>

  <template>
    <struct_name>Trick::CommandLineArguments</struct_name>
    <parameter>output_dir</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>input_file</next_sibling>
  </template>

  <template>
    <struct_name>Trick::CommandLineArguments</struct_name>
    <parameter>input_file</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::DMTCP</struct_name>
    <parameter>checkpoint_now</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>checkpoint_name</next_sibling>
  </template>

  <template>
    <struct_name>Trick::DMTCP</struct_name>
    <parameter>checkpoint_name</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::DataRecordDisk</struct_name>
    <parameter>buffering</parameter>
    <type_name>unsigned int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::DataRecordRing</struct_name>
    <parameter>buffering</parameter>
    <type_name>unsigned int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::DataRecordThreaded</struct_name>
    <parameter>cpu_number</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>buffering</next_sibling>
  </template>

  <template>
    <struct_name>Trick::DataRecordThreaded</struct_name>
    <parameter>buffering</parameter>
    <type_name>unsigned int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::DebugPause</struct_name>
    <parameter>debug_pause_flag</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::GetTimeOfDayClock</struct_name>
    <parameter>rt_clock_ratio</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPython</struct_name>
    <parameter>mtv_list</parameter>
    <type_name>Trick::IPPythonEvent</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>2</num_dimensions>
    <dim0>0</dim0>
    <dim1>0</dim1>
    <first_child>is_ip_event</first_child>
    <next_sibling>mtv_count</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPython</struct_name>
    <parameter>mtv_count</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>mtv_update_ticker</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPython</struct_name>
    <parameter>mtv_update_ticker</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>num_event_list</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPython</struct_name>
    <parameter>num_event_list</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>event_list_for_checkpoint</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPython</struct_name>
    <parameter>event_list_for_checkpoint</parameter>
    <type_name>Trick::IPPythonEvent</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>2</num_dimensions>
    <dim0>0</dim0>
    <dim1>0</dim1>
    <first_child>is_ip_event</first_child>
    <next_sibling>verify_input</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPython</struct_name>
    <parameter>verify_input</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>input_file</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPython</struct_name>
    <parameter>input_file</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPythonEvent</struct_name>
    <parameter>is_ip_event</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>is_user_event</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPythonEvent</struct_name>
    <parameter>is_user_event</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>before_after</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPythonEvent</struct_name>
    <parameter>before_after</parameter>
    <type_name>Trick::EventBeforeAfter</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>enumerated</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>target_job</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPythonEvent</struct_name>
    <parameter>target_job</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>cond_all</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPythonEvent</struct_name>
    <parameter>cond_all</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>fired</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPythonEvent</struct_name>
    <parameter>fired</parameter>
    <type_name>char</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>hold</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPythonEvent</struct_name>
    <parameter>hold</parameter>
    <type_name>char</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>ran</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPythonEvent</struct_name>
    <parameter>ran</parameter>
    <type_name>char</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>manual</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPythonEvent</struct_name>
    <parameter>manual</parameter>
    <type_name>char</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>manual_fired</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPythonEvent</struct_name>
    <parameter>manual_fired</parameter>
    <type_name>char</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>condition_count</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPythonEvent</struct_name>
    <parameter>condition_count</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>fired_count</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPythonEvent</struct_name>
    <parameter>fired_count</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>fired_time</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPythonEvent</struct_name>
    <parameter>fired_time</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>s</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>action_count</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPythonEvent</struct_name>
    <parameter>action_count</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>ran_count</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPythonEvent</struct_name>
    <parameter>ran_count</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>ran_time</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPythonEvent</struct_name>
    <parameter>ran_time</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>s</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>cond</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPythonEvent</struct_name>
    <parameter>cond</parameter>
    <type_name>Trick::condition_t</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>0</dim0>
    <first_child>enabled</first_child>
    <next_sibling>act</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPythonEvent</struct_name>
    <parameter>act</parameter>
    <type_name>Trick::action_t</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>0</dim0>
    <first_child>enabled</first_child>
    <next_sibling>name</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPythonEvent</struct_name>
    <parameter>name</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>active</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPythonEvent</struct_name>
    <parameter>active</parameter>
    <type_name>char</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>cycle</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPythonEvent</struct_name>
    <parameter>cycle</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>s</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>cycle_tics</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPythonEvent</struct_name>
    <parameter>cycle_tics</parameter>
    <type_name>long long</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>next_tics</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IPPythonEvent</struct_name>
    <parameter>next_tics</parameter>
    <type_name>long long</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::ITimer</struct_name>
    <parameter>enabled</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>active</next_sibling>
  </template>

  <template>
    <struct_name>Trick::ITimer</struct_name>
    <parameter>active</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>frame_time</next_sibling>
  </template>

  <template>
    <struct_name>Trick::ITimer</struct_name>
    <parameter>frame_time</parameter>
    <type_name>long long</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IntegLoopScheduler</struct_name>
    <parameter>cycle</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>s</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>pre_integ_jobs</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IntegLoopScheduler</struct_name>
    <parameter>pre_integ_jobs</parameter>
    <type_name>Trick::ScheduledJobQueue</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
    <next_sibling>deriv_jobs</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IntegLoopScheduler</struct_name>
    <parameter>deriv_jobs</parameter>
    <type_name>Trick::ScheduledJobQueue</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
    <next_sibling>integ_jobs</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IntegLoopScheduler</struct_name>
    <parameter>integ_jobs</parameter>
    <type_name>Trick::ScheduledJobQueue</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
    <next_sibling>dynamic_event_jobs</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IntegLoopScheduler</struct_name>
    <parameter>dynamic_event_jobs</parameter>
    <type_name>Trick::ScheduledJobQueue</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
    <next_sibling>post_integ_jobs</next_sibling>
  </template>

  <template>
    <struct_name>Trick::IntegLoopScheduler</struct_name>
    <parameter>post_integ_jobs</parameter>
    <type_name>Trick::ScheduledJobQueue</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::Integrator</struct_name>
    <parameter>num_state</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>intermediate_step</next_sibling>
  </template>

  <template>
    <struct_name>Trick::Integrator</struct_name>
    <parameter>intermediate_step</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>first_step_deriv</next_sibling>
  </template>

  <template>
    <struct_name>Trick::Integrator</struct_name>
    <parameter>first_step_deriv</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>last_step_deriv</next_sibling>
  </template>

  <template>
    <struct_name>Trick::Integrator</struct_name>
    <parameter>last_step_deriv</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>dt</next_sibling>
  </template>

  <template>
    <struct_name>Trick::Integrator</struct_name>
    <parameter>dt</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>state</next_sibling>
  </template>

  <template>
    <struct_name>Trick::Integrator</struct_name>
    <parameter>state</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>0</dim0>
    <next_sibling>deriv</next_sibling>
  </template>

  <template>
    <struct_name>Trick::Integrator</struct_name>
    <parameter>deriv</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>2</num_dimensions>
    <dim0>0</dim0>
    <dim1>0</dim1>
    <next_sibling>deriv2</next_sibling>
  </template>

  <template>
    <struct_name>Trick::Integrator</struct_name>
    <parameter>deriv2</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>2</num_dimensions>
    <dim0>0</dim0>
    <dim1>0</dim1>
    <next_sibling>state_ws</next_sibling>
  </template>

  <template>
    <struct_name>Trick::Integrator</struct_name>
    <parameter>state_ws</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>2</num_dimensions>
    <dim0>0</dim0>
    <dim1>0</dim1>
    <next_sibling>time</next_sibling>
  </template>

  <template>
    <struct_name>Trick::Integrator</struct_name>
    <parameter>time</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>time_0</next_sibling>
  </template>

  <template>
    <struct_name>Trick::Integrator</struct_name>
    <parameter>time_0</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>verbosity</next_sibling>
  </template>

  <template>
    <struct_name>Trick::Integrator</struct_name>
    <parameter>verbosity</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::JobData</struct_name>
    <parameter>name</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>disabled</next_sibling>
  </template>

  <template>
    <struct_name>Trick::JobData</struct_name>
    <parameter>disabled</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>complete</next_sibling>
  </template>

  <template>
    <struct_name>Trick::JobData</struct_name>
    <parameter>complete</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>handled</next_sibling>
  </template>

  <template>
    <struct_name>Trick::JobData</struct_name>
    <parameter>handled</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>cycle</next_sibling>
  </template>

  <template>
    <struct_name>Trick::JobData</struct_name>
    <parameter>cycle</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>start</next_sibling>
  </template>

  <template>
    <struct_name>Trick::JobData</struct_name>
    <parameter>start</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>stop</next_sibling>
  </template>

  <template>
    <struct_name>Trick::JobData</struct_name>
    <parameter>stop</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>job_class</next_sibling>
  </template>

  <template>
    <struct_name>Trick::JobData</struct_name>
    <parameter>job_class</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>job_class_name</next_sibling>
  </template>

  <template>
    <struct_name>Trick::JobData</struct_name>
    <parameter>job_class_name</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>system_job_class</next_sibling>
  </template>

  <template>
    <struct_name>Trick::JobData</struct_name>
    <parameter>system_job_class</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>phase</next_sibling>
  </template>

  <template>
    <struct_name>Trick::JobData</struct_name>
    <parameter>phase</parameter>
    <type_name>unsigned short</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>tag</next_sibling>
  </template>

  <template>
    <struct_name>Trick::JobData</struct_name>
    <parameter>tag</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>thread</next_sibling>
  </template>

  <template>
    <struct_name>Trick::JobData</struct_name>
    <parameter>thread</parameter>
    <type_name>unsigned int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>id</next_sibling>
  </template>

  <template>
    <struct_name>Trick::JobData</struct_name>
    <parameter>id</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>sim_object_id</next_sibling>
  </template>

  <template>
    <struct_name>Trick::JobData</struct_name>
    <parameter>sim_object_id</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>cycle_tics</next_sibling>
  </template>

  <template>
    <struct_name>Trick::JobData</struct_name>
    <parameter>cycle_tics</parameter>
    <type_name>long long</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>start_tics</next_sibling>
  </template>

  <template>
    <struct_name>Trick::JobData</struct_name>
    <parameter>start_tics</parameter>
    <type_name>long long</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>stop_tics</next_sibling>
  </template>

  <template>
    <struct_name>Trick::JobData</struct_name>
    <parameter>stop_tics</parameter>
    <type_name>long long</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>next_tics</next_sibling>
  </template>

  <template>
    <struct_name>Trick::JobData</struct_name>
    <parameter>next_tics</parameter>
    <type_name>long long</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MSConnect</struct_name>
    <parameter>sync_wait_limit</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>s</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::Master</struct_name>
    <parameter>enabled</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>num_slaves</next_sibling>
  </template>

  <template>
    <struct_name>Trick::Master</struct_name>
    <parameter>num_slaves</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MemoryManagerWrapper</struct_name>
    <parameter>safestore_period</parameter>
    <type_name>long long</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>safestore_time</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MemoryManagerWrapper</struct_name>
    <parameter>safestore_time</parameter>
    <type_name>long long</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>pre_init_checkpoint</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MemoryManagerWrapper</struct_name>
    <parameter>pre_init_checkpoint</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>post_init_checkpoint</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MemoryManagerWrapper</struct_name>
    <parameter>post_init_checkpoint</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>end_checkpoint</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MemoryManagerWrapper</struct_name>
    <parameter>end_checkpoint</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>safestore_enabled</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MemoryManagerWrapper</struct_name>
    <parameter>safestore_enabled</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>cpu_num</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MemoryManagerWrapper</struct_name>
    <parameter>cpu_num</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MessageCout</struct_name>
    <parameter>enabled</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>color</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MessageCout</struct_name>
    <parameter>color</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MessageFile</struct_name>
    <parameter>file_name</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>enabled</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MessageFile</struct_name>
    <parameter>enabled</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>color</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MessageFile</struct_name>
    <parameter>color</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MessagePublisher</struct_name>
    <parameter>sim_name</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MessageTCDevice</struct_name>
    <parameter>port</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>enabled</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MessageTCDevice</struct_name>
    <parameter>enabled</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>color</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MessageTCDevice</struct_name>
    <parameter>color</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>enabled</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>dry_run</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>dry_run</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>localhost_as_remote</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>localhost_as_remote</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>custom_slave_dispatch</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>custom_slave_dispatch</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>timeout</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>timeout</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>s</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>max_tries</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>max_tries</parameter>
    <type_name>unsigned int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>user_cmd_string</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>user_cmd_string</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>custom_pre_text</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>custom_pre_text</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>custom_post_text</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>custom_post_text</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>verbosity</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>verbosity</parameter>
    <type_name>Trick::MonteCarlo::Verbosity</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>enumerated</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>listen_device</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>listen_device</parameter>
    <type_name>TCDevice</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>disabled</first_child>
    <next_sibling>connection_device</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>connection_device</parameter>
    <type_name>TCDevice</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>disabled</first_child>
    <next_sibling>data_listen_device</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>data_listen_device</parameter>
    <type_name>TCDevice</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>disabled</first_child>
    <next_sibling>data_connection_device</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>data_connection_device</parameter>
    <type_name>TCDevice</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child>disabled</first_child>
    <next_sibling>num_slaves</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>num_slaves</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>slaves_head</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>slaves_head</parameter>
    <type_name>Trick::MonteSlave</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>2</num_dimensions>
    <dim0>0</dim0>
    <dim1>0</dim1>
    <first_child>id</first_child>
    <next_sibling>current_run</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>current_run</parameter>
    <type_name>unsigned int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>num_runs</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>num_runs</parameter>
    <type_name>unsigned int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>actual_num_runs</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>actual_num_runs</parameter>
    <type_name>unsigned int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>num_results</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>num_results</parameter>
    <type_name>unsigned int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>start_time</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>start_time</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>end_time</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>end_time</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>master_port</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>master_port</parameter>
    <type_name>unsigned int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>data_port</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>data_port</parameter>
    <type_name>unsigned int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>slave_id</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>slave_id</parameter>
    <type_name>unsigned int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>machine_name</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>machine_name</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>run_data_file</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>run_data_file</parameter>
    <type_name>_IO_FILE</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>run_header_file</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>run_header_file</parameter>
    <type_name>_IO_FILE</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>run_directory</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>run_directory</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>master_init_queue</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>master_init_queue</parameter>
    <type_name>Trick::ScheduledJobQueue</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
    <next_sibling>master_pre_queue</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>master_pre_queue</parameter>
    <type_name>Trick::ScheduledJobQueue</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
    <next_sibling>master_post_queue</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>master_post_queue</parameter>
    <type_name>Trick::ScheduledJobQueue</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
    <next_sibling>master_shutdown_queue</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>master_shutdown_queue</parameter>
    <type_name>Trick::ScheduledJobQueue</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
    <next_sibling>slave_init_queue</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>slave_init_queue</parameter>
    <type_name>Trick::ScheduledJobQueue</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
    <next_sibling>slave_pre_queue</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>slave_pre_queue</parameter>
    <type_name>Trick::ScheduledJobQueue</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
    <next_sibling>slave_post_queue</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>slave_post_queue</parameter>
    <type_name>Trick::ScheduledJobQueue</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
    <next_sibling>slave_shutdown_queue</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>slave_shutdown_queue</parameter>
    <type_name>Trick::ScheduledJobQueue</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>0</num_dimensions>
    <first_child></first_child>
    <next_sibling>attach_debugger</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>attach_debugger</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>debugger_command</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>debugger_command</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>enable_freeze</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>enable_freeze</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>stack_trace</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>stack_trace</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>trap_sigbus</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>trap_sigbus</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>trap_sigfpe</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>trap_sigfpe</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>trap_sigsegv</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>trap_sigsegv</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>rt_nap</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>rt_nap</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>software_frame</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>software_frame</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>s</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>software_frame_tics</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>software_frame_tics</parameter>
    <type_name>long long</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>freeze_frame</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>freeze_frame</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>s</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>freeze_frame_tics</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>freeze_frame_tics</parameter>
    <type_name>long long</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>next_frame_check_tics</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>next_frame_check_tics</parameter>
    <type_name>long long</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>mode</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>mode</parameter>
    <type_name>SIM_MODE</type_name>
    <io_attributes>OutputOnly</io_attributes>
    <units>--</units>
    <type_def>enumerated</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>terminate_time</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>terminate_time</parameter>
    <type_name>long long</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>time_tic_value</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>time_tic_value</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>1/s</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>time_tics</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>time_tics</parameter>
    <type_name>long long</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>freeze_command</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>freeze_command</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>sim_start</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>sim_start</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>s</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>time_last_pass_tics</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>time_last_pass_tics</parameter>
    <type_name>long long</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>job_call_time_tics</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>job_call_time_tics</parameter>
    <type_name>long long</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>num_classes</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>num_classes</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>num_all_jobs</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>num_all_jobs</parameter>
    <type_name>unsigned int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>all_jobs_for_checkpoint</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>all_jobs_for_checkpoint</parameter>
    <type_name>Trick::JobData</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>0</dim0>
    <first_child>name</first_child>
    <next_sibling>num_sim_objects</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>num_sim_objects</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>num_sim_objects_in_checkpoint</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>num_sim_objects_in_checkpoint</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>sim_objects_for_checkpoint</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>sim_objects_for_checkpoint</parameter>
    <type_name>Trick::SimObject</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>2</num_dimensions>
    <dim0>0</dim0>
    <dim1>0</dim1>
    <first_child></first_child>
    <next_sibling>version_date_tag</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>version_date_tag</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>build_date</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>build_date</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>current_version</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteCarlo</struct_name>
    <parameter>current_version</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteRun</struct_name>
    <parameter>id</parameter>
    <type_name>unsigned int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>num_tries</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteRun</struct_name>
    <parameter>num_tries</parameter>
    <type_name>unsigned int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>start_time</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteRun</struct_name>
    <parameter>start_time</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>end_time</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteRun</struct_name>
    <parameter>end_time</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>exit_status</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteRun</struct_name>
    <parameter>exit_status</parameter>
    <type_name>Trick::MonteRun::ExitStatus</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>enumerated</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteSlave</struct_name>
    <parameter>id</parameter>
    <type_name>unsigned int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>state</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteSlave</struct_name>
    <parameter>state</parameter>
    <type_name>Trick::MonteSlave::State</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>enumerated</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>machine_name</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteSlave</struct_name>
    <parameter>machine_name</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>port</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteSlave</struct_name>
    <parameter>port</parameter>
    <type_name>unsigned int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>current_run</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteSlave</struct_name>
    <parameter>current_run</parameter>
    <type_name>Trick::MonteRun</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>0</dim0>
    <first_child>id</first_child>
    <next_sibling>num_dispatches</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteSlave</struct_name>
    <parameter>num_dispatches</parameter>
    <type_name>unsigned int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>num_results</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteSlave</struct_name>
    <parameter>num_results</parameter>
    <type_name>unsigned int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>cpu_time</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteSlave</struct_name>
    <parameter>cpu_time</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>remote_shell</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteSlave</struct_name>
    <parameter>remote_shell</parameter>
    <type_name>Trick::RemoteShell</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>enumerated</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>user_remote_shell</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteSlave</struct_name>
    <parameter>user_remote_shell</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>remote_shell_args</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteSlave</struct_name>
    <parameter>remote_shell_args</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>multiplier</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteSlave</struct_name>
    <parameter>multiplier</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>sim_path</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteSlave</struct_name>
    <parameter>sim_path</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>S_main_name</next_sibling>
  </template>

  <template>
    <struct_name>Trick::MonteSlave</struct_name>
    <parameter>S_main_name</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::RealtimeSync</struct_name>
    <parameter>enable_flag</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>disable_flag</next_sibling>
  </template>

  <template>
    <struct_name>Trick::RealtimeSync</struct_name>
    <parameter>disable_flag</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>active</next_sibling>
  </template>

  <template>
    <struct_name>Trick::RealtimeSync</struct_name>
    <parameter>active</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>rt_freeze_frame_tics</next_sibling>
  </template>

  <template>
    <struct_name>Trick::RealtimeSync</struct_name>
    <parameter>rt_freeze_frame_tics</parameter>
    <type_name>long long</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>s</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>frame_overrun_cnt</next_sibling>
  </template>

  <template>
    <struct_name>Trick::RealtimeSync</struct_name>
    <parameter>frame_overrun_cnt</parameter>
    <type_name>unsigned int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>total_overrun</next_sibling>
  </template>

  <template>
    <struct_name>Trick::RealtimeSync</struct_name>
    <parameter>total_overrun</parameter>
    <type_name>unsigned int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>rt_max_overrun_cnt</next_sibling>
  </template>

  <template>
    <struct_name>Trick::RealtimeSync</struct_name>
    <parameter>rt_max_overrun_cnt</parameter>
    <type_name>unsigned int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>rt_max_overrun_time</next_sibling>
  </template>

  <template>
    <struct_name>Trick::RealtimeSync</struct_name>
    <parameter>rt_max_overrun_time</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>s</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>rt_max_overrun_time_tics</next_sibling>
  </template>

  <template>
    <struct_name>Trick::RealtimeSync</struct_name>
    <parameter>rt_max_overrun_time_tics</parameter>
    <type_name>long long</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>rt_overrun_freeze</next_sibling>
  </template>

  <template>
    <struct_name>Trick::RealtimeSync</struct_name>
    <parameter>rt_overrun_freeze</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>frame_sched_time</next_sibling>
  </template>

  <template>
    <struct_name>Trick::RealtimeSync</struct_name>
    <parameter>frame_sched_time</parameter>
    <type_name>long long</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>frame_overrun_time</next_sibling>
  </template>

  <template>
    <struct_name>Trick::RealtimeSync</struct_name>
    <parameter>frame_overrun_time</parameter>
    <type_name>long long</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>last_clock_time</next_sibling>
  </template>

  <template>
    <struct_name>Trick::RealtimeSync</struct_name>
    <parameter>last_clock_time</parameter>
    <type_name>long long</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>tics_per_sec</next_sibling>
  </template>

  <template>
    <struct_name>Trick::RealtimeSync</struct_name>
    <parameter>tics_per_sec</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>rt_clock</next_sibling>
  </template>

  <template>
    <struct_name>Trick::RealtimeSync</struct_name>
    <parameter>rt_clock</parameter>
    <type_name>Trick::Clock</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>0</dim0>
    <first_child>rt_clock_ratio</first_child>
    <next_sibling>sleep_timer</next_sibling>
  </template>

  <template>
    <struct_name>Trick::RealtimeSync</struct_name>
    <parameter>sleep_timer</parameter>
    <type_name>Trick::Timer</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>0</dim0>
    <first_child>enabled</first_child>
    <next_sibling>freeze_shutdown</next_sibling>
  </template>

  <template>
    <struct_name>Trick::RealtimeSync</struct_name>
    <parameter>freeze_shutdown</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::Slave</struct_name>
    <parameter>enabled</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>sync_error_terminate</next_sibling>
  </template>

  <template>
    <struct_name>Trick::Slave</struct_name>
    <parameter>sync_error_terminate</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>sync_wait_limit</next_sibling>
  </template>

  <template>
    <struct_name>Trick::Slave</struct_name>
    <parameter>sync_wait_limit</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>s</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>connection</next_sibling>
  </template>

  <template>
    <struct_name>Trick::Slave</struct_name>
    <parameter>connection</parameter>
    <type_name>Trick::MSConnect</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>data_structure_address</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>0</dim0>
    <first_child>sync_wait_limit</first_child>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::Timer</struct_name>
    <parameter>enabled</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>active</next_sibling>
  </template>

  <template>
    <struct_name>Trick::Timer</struct_name>
    <parameter>active</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>frame_time</next_sibling>
  </template>

  <template>
    <struct_name>Trick::Timer</struct_name>
    <parameter>frame_time</parameter>
    <type_name>long long</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::UnitTest</struct_name>
    <parameter>enabled</parameter>
    <type_name>bool</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>file_name</next_sibling>
  </template>

  <template>
    <struct_name>Trick::UnitTest</struct_name>
    <parameter>file_name</parameter>
    <type_name>std::string</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::action_t</struct_name>
    <parameter>enabled</parameter>
    <type_name>char</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>ran</next_sibling>
  </template>

  <template>
    <struct_name>Trick::action_t</struct_name>
    <parameter>ran</parameter>
    <type_name>char</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>ran_count</next_sibling>
  </template>

  <template>
    <struct_name>Trick::action_t</struct_name>
    <parameter>ran_count</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>ran_time</next_sibling>
  </template>

  <template>
    <struct_name>Trick::action_t</struct_name>
    <parameter>ran_time</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>s</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>str</next_sibling>
  </template>

  <template>
    <struct_name>Trick::action_t</struct_name>
    <parameter>str</parameter>
    <type_name>char</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>0</dim0>
    <next_sibling>comment</next_sibling>
  </template>

  <template>
    <struct_name>Trick::action_t</struct_name>
    <parameter>comment</parameter>
    <type_name>char</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>0</dim0>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>Trick::condition_t</struct_name>
    <parameter>enabled</parameter>
    <type_name>char</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>hold</next_sibling>
  </template>

  <template>
    <struct_name>Trick::condition_t</struct_name>
    <parameter>hold</parameter>
    <type_name>char</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>fired</next_sibling>
  </template>

  <template>
    <struct_name>Trick::condition_t</struct_name>
    <parameter>fired</parameter>
    <type_name>char</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>fired_count</next_sibling>
  </template>

  <template>
    <struct_name>Trick::condition_t</struct_name>
    <parameter>fired_count</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>fired_time</next_sibling>
  </template>

  <template>
    <struct_name>Trick::condition_t</struct_name>
    <parameter>fired_time</parameter>
    <type_name>double</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>s</units>
    <type_def>other</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>str</next_sibling>
  </template>

  <template>
    <struct_name>Trick::condition_t</struct_name>
    <parameter>str</parameter>
    <type_name>char</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>0</dim0>
    <next_sibling>comment</next_sibling>
  </template>

  <template>
    <struct_name>Trick::condition_t</struct_name>
    <parameter>comment</parameter>
    <type_name>char</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>0</dim0>
    <next_sibling>NULL</next_sibling>
  </template>

  <template>
    <struct_name>_TrickErrorHndlr</struct_name>
    <parameter>report_level</parameter>
    <type_name>TrickErrorLevel</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>enumerated</type_def>
    <num_dimensions>0</num_dimensions>
    <next_sibling>use_send_hs</next_sibling>
  </template>

  <template>
    <struct_name>_TrickErrorHndlr</struct_name>
    <parameter>use_send_hs</parameter>
    <type_name>int</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>9</dim0>
    <next_sibling>report_stream</next_sibling>
  </template>

  <template>
    <struct_name>_TrickErrorHndlr</struct_name>
    <parameter>report_stream</parameter>
    <type_name>_IO_FILE</type_name>
    <io_attributes>InputAndOutput</io_attributes>
    <units>--</units>
    <type_def>other</type_def>
    <num_dimensions>1</num_dimensions>
    <dim0>9</dim0>
    <next_sibling>NULL</next_sibling>
  </template>

  <enumeration>
    <enumeration_name>SIM_MODE</enumeration_name>
    <pair>
      <label>Initialization</label>
      <value>0</value>
    </pair>
    <pair>
      <label>Freeze</label>
      <value>1</value>
    </pair>
    <pair>
      <label>Step</label>
      <value>4</value>
    </pair>
    <pair>
      <label>Run</label>
      <value>5</value>
    </pair>
    <pair>
      <label>ExitMode</label>
      <value>6</value>
    </pair>
  </enumeration>

  <enumeration>
    <enumeration_name>TCCommBlocking</enumeration_name>
    <pair>
      <label>TC_COMM_BLOCKIO</label>
      <value>0</value>
    </pair>
    <pair>
      <label>TC_COMM_NOBLOCKIO</label>
      <value>1</value>
    </pair>
    <pair>
      <label>TC_COMM_TIMED_BLOCKIO</label>
      <value>2</value>
    </pair>
    <pair>
      <label>TC_COMM_ALL_OR_NOTHING</label>
      <value>3</value>
    </pair>
  </enumeration>

  <enumeration>
    <enumeration_name>TCCommFlag</enumeration_name>
    <pair>
      <label>TC_COMM_FALSE</label>
      <value>0</value>
    </pair>
    <pair>
      <label>TC_COMM_TRUE</label>
      <value>1</value>
    </pair>
    <pair>
      <label>TC_COMM_NO</label>
      <value>0</value>
    </pair>
    <pair>
      <label>TC_COMM_YES</label>
      <value>1</value>
    </pair>
    <pair>
      <label>TC_COMM_DISABLED</label>
      <value>0</value>
    </pair>
    <pair>
      <label>TC_COMM_ENABLED</label>
      <value>1</value>
    </pair>
  </enumeration>

  <enumeration>
    <enumeration_name>Trick::EventBeforeAfter</enumeration_name>
    <pair>
      <label>Trick::EVENT_NOTARGET</label>
      <value>0</value>
    </pair>
    <pair>
      <label>Trick::EVENT_BEFORETARGET</label>
      <value>1</value>
    </pair>
    <pair>
      <label>Trick::EVENT_AFTERTARGET</label>
      <value>2</value>
    </pair>
  </enumeration>

  <enumeration>
    <enumeration_name>Trick::MonteCarlo::Verbosity</enumeration_name>
    <pair>
      <label>Trick::MonteCarlo::NONE</label>
      <value>0</value>
    </pair>
    <pair>
      <label>Trick::MonteCarlo::ERROR</label>
      <value>1</value>
    </pair>
    <pair>
      <label>Trick::MonteCarlo::INFORMATIONAL</label>
      <value>2</value>
    </pair>
    <pair>
      <label>Trick::MonteCarlo::ALL</label>
      <value>3</value>
    </pair>
  </enumeration>

  <enumeration>
    <enumeration_name>Trick::MonteRun::ExitStatus</enumeration_name>
    <pair>
      <label>Trick::MonteRun::INCOMPLETE</label>
      <value>0</value>
    </pair>
    <pair>
      <label>Trick::MonteRun::COMPLETE</label>
      <value>1</value>
    </pair>
    <pair>
      <label>Trick::MonteRun::CORED</label>
      <value>2</value>
    </pair>
    <pair>
      <label>Trick::MonteRun::TIMEDOUT</label>
      <value>3</value>
    </pair>
    <pair>
      <label>Trick::MonteRun::NO_PERM</label>
      <value>4</value>
    </pair>
    <pair>
      <label>Trick::MonteRun::UNKNOWN</label>
      <value>5</value>
    </pair>
  </enumeration>

  <enumeration>
    <enumeration_name>Trick::MonteSlave::State</enumeration_name>
    <pair>
      <label>Trick::MonteSlave::UNINITIALIZED</label>
      <value>0</value>
    </pair>
    <pair>
      <label>Trick::MonteSlave::INITIALIZING</label>
      <value>1</value>
    </pair>
    <pair>
      <label>Trick::MonteSlave::READY</label>
      <value>2</value>
    </pair>
    <pair>
      <label>Trick::MonteSlave::RUNNING</label>
      <value>3</value>
    </pair>
    <pair>
      <label>Trick::MonteSlave::STOPPING</label>
      <value>4</value>
    </pair>
    <pair>
      <label>Trick::MonteSlave::STOPPED</label>
      <value>5</value>
    </pair>
    <pair>
      <label>Trick::MonteSlave::FINISHED</label>
      <value>6</value>
    </pair>
    <pair>
      <label>Trick::MonteSlave::UNRESPONSIVE_RUNNING</label>
      <value>7</value>
    </pair>
    <pair>
      <label>Trick::MonteSlave::UNRESPONSIVE_STOPPING</label>
      <value>8</value>
    </pair>
    <pair>
      <label>Trick::MonteSlave::DISCONNECTED</label>
      <value>9</value>
    </pair>
  </enumeration>

  <enumeration>
    <enumeration_name>Trick::RemoteShell</enumeration_name>
    <pair>
      <label>Trick::TRICK_SSH</label>
      <value>0</value>
    </pair>
    <pair>
      <label>Trick::TRICK_RSH</label>
      <value>1</value>
    </pair>
    <pair>
      <label>Trick::TRICK_USER_REMOTE_SH</label>
      <value>2</value>
    </pair>
  </enumeration>

  <enumeration>
    <enumeration_name>TrickErrorLevel</enumeration_name>
    <pair>
      <label>TRICK_ERROR_ALL</label>
      <value>0</value>
    </pair>
    <pair>
      <label>TRICK_ERROR_TRIVIAL</label>
      <value>1</value>
    </pair>
    <pair>
      <label>TRICK_ERROR_ADVISORY</label>
      <value>2</value>
    </pair>
    <pair>
      <label>TRICK_ERROR_CAUTION</label>
      <value>3</value>
    </pair>
    <pair>
      <label>TRICK_ERROR_WARNING</label>
      <value>4</value>
    </pair>
    <pair>
      <label>TRICK_ERROR_ALERT</label>
      <value>5</value>
    </pair>
    <pair>
      <label>TRICK_ERROR_FATAL</label>
      <value>6</value>
    </pair>
    <pair>
      <label>TRICK_ERROR_ABORT</label>
      <value>7</value>
    </pair>
    <pair>
      <label>TRICK_ERROR_SILENT</label>
      <value>8</value>
    </pair>
  </enumeration>

</sie>
