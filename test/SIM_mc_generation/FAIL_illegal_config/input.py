monte_carlo.mc_master.activate("FAIL_illegal_config")
monte_carlo.mc_master.set_num_runs(1)

print('**********************************************************************************\n' +
'this message is expected:\n' +
'  DIAGNOSTIC: Fatal Error Illegal configuration\n' +
'For variable test.x_normal the specified minimum allowable value (6) >= the specified maximum allowable value (3).\n' +
'One or both of the limits must be changed to generate a random value.\n' +
'**********************************************************************************')


mc_var = trick.MonteCarloVariableRandomNormal( "test.x_normal", 2, 10, 2)
# flip low and high values to generate the desired error
mc_var.truncate_low(6, trick.MonteCarloVariableRandomNormal.Absolute)
mc_var.truncate_high(3, trick.MonteCarloVariableRandomNormal.Absolute)
monte_carlo.mc_master.add_variable(mc_var)

trick.stop(1)
