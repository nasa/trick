monte_carlo.mc_master.activate("RUN_WARN_config_error")
monte_carlo.mc_master.set_num_runs(1)

print('***************************************************************************************')
print('this message is expected:')
print('  Warning Configuration error')
print('  Zero truncation specified for variable test.x_normal which will produce a fixed point')
print('***************************************************************************************')


mc_var = trick.MonteCarloVariableRandomNormal( "test.x_normal", 24858569, 10, 2)

# this call generates the warning!
mc_var.truncate(0.0)

# fix up the values before generating the assignment to avoid spurrious errors
mc_var.truncate(7.0, 13.0, trick.MonteCarloVariableRandomNormal.Absolute)
monte_carlo.mc_master.add_variable(mc_var)

trick.stop(1)
