monte_carlo.mc_master.activate("RUN_ERROR_invalid_call")
monte_carlo.mc_master.set_num_runs(1)

print('*********************************************************************')
print('this message is expected:')
print('  Error Invalid call')
print('  Attempted to register a dependent identified with NULL pointer with')
print('  the MonteCarloVariableFile for variable test.x_file_lookup[0].')
print('  This is not a valid action.')
print('  Registration failed, exiting without action.')
print('*********************************************************************')


mc_var = trick.MonteCarloVariableFile( "test.x_file_lookup[0]",
                                       "Modified_data/datafile.txt",
                                       3)
# the next command is the source of the error!
mc_var.register_dependent(None)
monte_carlo.mc_master.add_variable(mc_var)

trick.stop(1)
