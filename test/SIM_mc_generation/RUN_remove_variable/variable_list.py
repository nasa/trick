
mc_var = trick.MonteCarloVariableRandomUniform( "test.x_uniform", 0, 10, 20)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloVariableFixed( "test.x_fixed_value_int", 7)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)
