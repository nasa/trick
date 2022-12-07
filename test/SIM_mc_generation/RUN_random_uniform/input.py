monte_carlo.mc_master.activate("RUN_random_uniform")
# Use 10 runs for regression comparison; use more (10,000) for confirming
# statistical distribution.
monte_carlo.mc_master.set_num_runs(10)

# generate random uniformly distributed floating point values from 100.0 to 100,000.0
mc_var = trick.MonteCarloVariableRandomUniform( "test.x_uniform", 77545, 100.0, 100000.0)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

# generate random uniformly distributed integer values from 100.0 to 100,000.0
mc_var = trick.MonteCarloVariableRandomUniformInt( "test.x_integer", 77001, 100, 100000)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

trick.stop(1)
