monte_carlo.mc_master.activate("RUN_random_normal__untruncate")
monte_carlo.mc_master.set_num_runs(10)

# generate a set of numbers
mc_var = trick.MonteCarloVariableRandomNormal( "test.x_normal_trunc[0]", 2, 10, 2)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloVariableRandomNormal( "test.x_normal_trunc[1]", 2, 10, 2)
# signal an absolute truncation
mc_var.truncate(8, 12, trick.MonteCarloVariableRandomNormal.Absolute)
# changed my mind. no longer wish to truncate.
# this method turns off 'truncated_low' and 'truncated_high' indicators, leaving
# the original variable values alone!
# NOTE: the two values in this sim should match!
#
# code coverage for untruncate() method, mc_variable_random_normal.cc, lines 204-205
mc_var.untruncate()
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

trick.stop(1)
