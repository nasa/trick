monte_carlo.mc_master.activate("RUN_WARN_no_string_values")
monte_carlo.mc_master.set_num_runs(1)

print('*********************************************************************************')
print('this message is expected:')
print('No values for MonteCarloVariableRandomStringSet')
print('Length of values vector is zero for variable: test.x_string, Did you forget to call add_string()?')
print('*********************************************************************************')


# add a string variable but forget to give it options to randomize
mc_var = trick.MonteCarloVariableRandomStringSet( "test.x_string", 3)
#mc_var.add_string("\"ABC\"")
#mc_var.add_string("\"DEF\"")
#mc_var.add_string("'GHIJKL'")
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)
monte_carlo.mc_master.generate_meta_data = True

