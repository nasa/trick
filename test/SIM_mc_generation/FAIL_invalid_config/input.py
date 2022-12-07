monte_carlo.mc_master.activate("FAIL_invalid_config")

print('******************************************************************************************')
print('this message is expected:')
print('    DIAGNOSTIC: Fatal Error Invalid configuration')
print('  Error in attempting to make test.x_file_lookup[1] be dependent on test.x_file_lookup[0].')
print('  test.x_file_lookup[1] cannot be marked as dependent when it has dependencies of its own.')
print('  The dependency hierarchy can only be one level deep.')
print('******************************************************************************************')


mc_var = trick.MonteCarloVariableFile( "test.x_file_lookup[0]",
                                       "Modified_data/datafile.txt",
                                       3)
monte_carlo.mc_master.add_variable(mc_var)

mc_var2 = trick.MonteCarloVariableFile( "test.x_file_lookup[1]",
                                        "Modified_data/datafile.txt",
                                        3)
# the next command is the source of the error!
mc_var2.register_dependent(mc_var)
monte_carlo.mc_master.add_variable(mc_var2)

trick.stop(1)
