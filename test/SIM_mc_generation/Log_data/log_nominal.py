dr_group = trick.sim_services.DRAscii("test_data")
dr_group.set_cycle(1)
dr_group.freq = trick.sim_services.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)

dr_group.add_variable( "test.x_uniform")
dr_group.add_variable( "test.x_normal")
for ii in range(5):
  dr_group.add_variable( "test.x_normal_trunc[%d]" %ii)
dr_group.add_variable( "test.x_normal_length")
dr_group.add_variable( "test.x_integer")
dr_group.add_variable( "test.x_line_command")
for ii in range(3):
  dr_group.add_variable( "test.x_file_command[%d]" %ii)
dr_group.add_variable( "test.x_boolean")
dr_group.add_variable( "test.x_file_lookup")
dr_group.add_variable( "test.x_fixed_value_double")
dr_group.add_variable( "test.x_fixed_value_int")
dr_group.add_variable( "test.x_semi_fixed_value")
dr_group.add_variable( "test.x_sdefine_routine_called")
