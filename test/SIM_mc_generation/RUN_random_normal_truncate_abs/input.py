monte_carlo.mc_master.activate("RUN_random_normal_truncate_abs")
# Use 10 runs for regression comparison; use more (10,000) for confirming
# statistical distribution.
monte_carlo.mc_master.set_num_runs(10)

# should keep values between -10 and 10, exclusive
# this one calls 'truncate_low(-10)' and 'truncate_high(10)' to establish
# truncation bounds so we need to pivot around ZERO. Otherwise, if mean
# is not ZERO, ex. 25, the truncation bounds would be -25 and 25 which
# does not match the desired values of +/- 10 of mean.
mc_var = trick.MonteCarloVariableRandomNormal( "test.x_normal_trunc[0]", 11122, 0, 5)
mc_var.truncate(10, trick.MonteCarloVariableRandomNormal.Absolute)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

# should keep values 72.5 thru 85.0, exclusive
mc_var = trick.MonteCarloVariableRandomNormal( "test.x_normal_trunc[1]", 77546, 75, 5)
mc_var.truncate(72.5, 85, trick.MonteCarloVariableRandomNormal.Absolute)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

# should keep values greater than 90.0
mc_var = trick.MonteCarloVariableRandomNormal( "test.x_normal_trunc[2]", 60540, 100, 5)
mc_var.truncate_low(90, trick.MonteCarloVariableRandomNormal.Absolute)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

# should keep values less than 135.0
mc_var = trick.MonteCarloVariableRandomNormal( "test.x_normal_trunc[3]", 77077, 125, 5)
mc_var.truncate_high(135, trick.MonteCarloVariableRandomNormal.Absolute)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

trick.stop(1)
