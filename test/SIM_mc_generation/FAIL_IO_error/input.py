monte_carlo.mc_master.activate("FAIL_IO_error")

print('*********************************************************************')
print('this message is expected:')
print('    DIAGNOSTIC: Fatal Error I/O error')
print('  Unable to open file Modified_data/nonexistent_file.txt for reading.')
print('  Required for variable test.x_file_lookup[0].')
print('*********************************************************************')


# try to open a non-existant file
# code coverage for 'mc_variable_file.cc', lines 71-76.
mc_var = trick.MonteCarloVariableFile( "test.x_file_lookup[0]",
                                       "Modified_data/nonexistent_file.txt",
                                       3)
monte_carlo.mc_master.add_variable(mc_var)

trick.stop(1)
