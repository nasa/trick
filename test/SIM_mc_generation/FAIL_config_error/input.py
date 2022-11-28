monte_carlo.mc_master.activate("FAIL_config_error")

print('************************************************************************************')
print('this message is expected:')
print('    DIAGNOSTIC: Fatal Error Configuration Error')
print('  In configuring the file for variable test.x_file_lookup[0], it was identified that')
print('  it was specified to draw data from column 4, but that the first')
print('  column was identified as having index 7.')
print('************************************************************************************')


# generate the error
mc_var = trick.MonteCarloVariableFile( "test.x_file_lookup[0]",
                                       "Modified_data/datafile.txt",
                                       4,
                                       7)
monte_carlo.mc_master.add_variable(mc_var)

trick.stop(1)
