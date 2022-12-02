monte_carlo.mc_master.activate("RUN_ERROR_file_inconsistent_skip")
monte_carlo.mc_master.set_num_runs(1)

print('*********************************************************************')
print('these messages are expected:')
print('  Error Invalid configuration')
print('  It is not permissible for two variables looking at the same file to')
print('  operate under different line-selection criteria.')
print('  test.x_file_lookup[1]')
print('  will be switched to the behavior of')
print('  test.x_file_lookup_rl[0],')
print('  which has a setting for the maximum number of lines to skip of 3')
print('')
print('  Error Invalid configuration')
print('  It is not permissible for two variables looking at the same file to')
print('  operate under different line-selection criteria.')
print('  test.x_file_lookup[2]')
print('  will be switched to the behavior of')
print('  test.x_file_lookup_rl[0],')
print('  which has a setting for the maximum number of lines to skip of 3')
print('*********************************************************************')


mc_var = trick.MonteCarloVariableFile( "test.x_file_lookup[0]",
                                        "Modified_data/datafile.txt",
                                        3)
mc_var.thisown = False
mc_var.max_skip = 3
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloVariableFile( "test.x_file_lookup[1]",
                                        "Modified_data/datafile.txt",
                                        2)
mc_var.thisown = False
mc_var.max_skip = 2
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloVariableFile( "test.x_file_lookup[2]",
                                        "Modified_data/datafile.txt",
                                        1)
mc_var.thisown = False
mc_var.max_skip = 1
monte_carlo.mc_master.add_variable(mc_var)


trick.stop(1)
