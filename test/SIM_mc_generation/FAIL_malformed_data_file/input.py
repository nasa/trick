monte_carlo.mc_master.activate("FAIL_malformed_data_file")
monte_carlo.mc_master.set_num_runs(1)

print('**************************************************************************************************')
print('this message is expected:')
print('    DIAGNOSTIC: Fatal Error Malformed data file')
print('  Data file for variable test.x_file_lookup[0] includes this line:')
print('  0 1 2 3 4')
print('  Which has only 5 values.')
print('  Variable test.x_file_lookup[0] uses the value from position 9, which does not exist in this line')
print('**************************************************************************************************')


# generate the error
mc_var = trick.MonteCarloVariableFile( "test.x_file_lookup[0]",
                                       "Modified_data/datafile.txt",
                                       9)
monte_carlo.mc_master.add_variable(mc_var)

trick.stop(1)
