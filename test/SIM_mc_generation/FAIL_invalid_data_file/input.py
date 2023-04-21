monte_carlo.mc_master.activate("FAIL_invalid_data_file")

print('*****************************************************************************')
print('this message is expected:')
print('    DIAGNOSTIC: Fatal Error Invalid data file')
print('  Data file Modified_data/empty_file.txt contains no recognized lines of data')
print('  Required for variable test.x_file_lookup[0].')
print('*****************************************************************************')


# try to open an empty file
mc_var = trick.MonteCarloVariableFile( "test.x_file_lookup[0]",
                                       "Modified_data/empty_file.txt",
                                       3)
monte_carlo.mc_master.add_variable(mc_var)

trick.stop(1)
