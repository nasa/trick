monte_carlo.mc_master.activate("RUN_ERROR_invalid_sequencing")
monte_carlo.mc_master.set_num_runs(1)

print('***********************************************************************************')
print('this message is expected:')
print('  Error Invalid sequencing')
print('  For variable test.x_semi_fixed_value, the necessary pre-dispersion to obtain the')
print('  random value for assignment has not been completed.')
print('  Cannot generate the assignment for this variable.')
print('***********************************************************************************')


mc_var = trick.MonteCarloVariableSemiFixed("test.x_semi_fixed_value",
                                            test.mc_var_file)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

trick.stop(1)
