monte_carlo.mc_master.activate("RUN_WARN_invalid_name")
monte_carlo.mc_master.set_num_runs(1)

print('*********************************************************************************')
print('this message is expected:')
print('  Warning Invalid name')
print('  Attempt to remove MonteCarlo variable with name monte_carlo.not_found_variable FAILED.')
print('  Did not find a variable with that name.')
print('*********************************************************************************')


# give the simulation something to do
# add a variable into memory so list is not empty when we go to
# remove a non-existant variable from memory.
mc_var = trick.MonteCarloVariableRandomUniform( "test.x_uniform", 0, 10, 20)
monte_carlo.mc_master.add_variable(mc_var)

# Trigger the "Invalid Name" warning in MonteCarloMaster
# by calling remove_variable() to remove a non-existant variable name
monte_carlo.mc_master.remove_variable("monte_carlo.not_found_variable")

trick.stop(1)
