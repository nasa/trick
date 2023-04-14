monte_carlo.mc_master.activate("RUN_random_normal_untruncated")
# Use 10 runs for regression comparison; use more (10,000) for confirming
# statistical distribution.
monte_carlo.mc_master.set_num_runs(10)

# normal distribution from approximately -17.5 to 18.1
mc_var = trick.MonteCarloVariableRandomNormal( "test.x_normal_trunc[0]", 11122, 0, 5)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

# normal distribution from approximately 53.1 to 94.5
mc_var = trick.MonteCarloVariableRandomNormal( "test.x_normal_trunc[1]", 77546, 75, 5)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

# normal distribution from approximately 81.4 to 119.75
mc_var = trick.MonteCarloVariableRandomNormal( "test.x_normal_trunc[2]", 60540, 100, 5)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

# normal distribution from approximately 106.3 to 144.7
mc_var = trick.MonteCarloVariableRandomNormal( "test.x_normal_trunc[3]", 77077, 125, 5)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

trick.stop(1)
