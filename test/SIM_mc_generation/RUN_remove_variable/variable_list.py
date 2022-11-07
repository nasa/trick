
mc_var = trick.MonteCarloVariableRandomUniform( "monte_carlo.x_uniform", 0, 10, 20)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloVariableFixed( "monte_carlo.x_fixed_value_int", 7)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)
