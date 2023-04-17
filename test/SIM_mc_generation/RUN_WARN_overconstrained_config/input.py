monte_carlo.mc_master.activate("RUN_WARN_overconstrained_config")
monte_carlo.mc_master.set_num_runs(1)

print('*******************************************************')
print('these messages are expected:')
print('  Warning Overconstrained configuration')
print('  For variable The distribution collapses to a point.')
print('   the specified minimum allowable value and')
print('  the specified maximum allowable value are equal (14).')
print('*******************************************************')


mc_var = trick.MonteCarloVariableRandomNormal( "test.x_normal", 2, 10, 2)
# the next two commands are neded to produce the warning!
mc_var.truncate_low(2.0)
mc_var.truncate_high(2.0)
monte_carlo.mc_master.add_variable(mc_var)

trick.stop(1)
