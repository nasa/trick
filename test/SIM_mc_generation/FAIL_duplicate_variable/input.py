monte_carlo.mc_master.activate("FAIL_duplicate_variable")

print('************************************************************')
print('this message is expected:')
print('    DIAGNOSTIC: Fatal Error Duplicated variable.')
print('  Attempted to add two settings for variable test.x_uniform.')
print('  Terminating to allow resolution of which setting to use.')
print('************************************************************')


mc_var = trick.MonteCarloVariableRandomUniform( "test.x_uniform", 0, 10, 20)
monte_carlo.mc_master.add_variable(mc_var)
# Add the variable twice to trigger the "Duplicated variable" fail in
# MonteCarloMaster::add_variable
monte_carlo.mc_master.add_variable(mc_var)

trick.stop(1)
