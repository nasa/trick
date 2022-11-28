monte_carlo.mc_master.activate("RUN_ERROR_random_value_truncation")
monte_carlo.mc_master.set_num_runs(2)

print('******************************************************************************************')
print('multiple error messages are expected:')
print('  Error Random value truncation failure')
print('  Could not generate a value for test.x_normal_trunc[#] within the specified domain within')
print('  the specified maximum number of tries (1).')
print('  Assuming a value equal to:')
print('   - midpoint value for a distribution truncated at both ends')
print('   - truncation value for a distribution truncated at only one end.')
print('')
print('NOTE: three tests are included here to test different code path after error')
print('      message is emitted')
print('******************************************************************************************')


# give some crazy initial values to the class
mc_var = trick.MonteCarloVariableRandomNormal( "test.x_normal_trunc[0]", 9956453, 10, 3.5)
mc_var.thisown = False
# give very small truncate_low & truncate_high distrubution values
mc_var.truncate_low(0.1)
mc_var.truncate_high(0.2)
# lower max_num_tries to an unreasonable # to generate the desired error
mc_var.max_num_tries = 1
monte_carlo.mc_master.add_variable(mc_var)

# note: this test also covers mc_variable_random_normal.cc lines 84-86
#       in the same code section after the message


# give some crazy initial values to the class
mc_var = trick.MonteCarloVariableRandomNormal( "test.x_normal_trunc[1]", 99565644453, 10, 3.5)
mc_var.thisown = False
# give very small truncate_low distrubution value
mc_var.truncate_low(0.1)
# lower max_num_tries to an unreasonable # to generate the desired error
mc_var.max_num_tries = 1
monte_carlo.mc_master.add_variable(mc_var)

# note: this test also covers mc_variable_random_normal.cc lines 78-80
#       in the same code section after the message


# give some crazy initial values to the class
mc_var = trick.MonteCarloVariableRandomNormal( "test.x_normal_trunc[2]", 99565644453, 10, 3.5)
mc_var.thisown = False
# give negative truncate_high distrubution value
mc_var.truncate_high(-50.2)
# lower max_num_tries to an unreasonable # to generate the desired error
mc_var.max_num_tries = 1
monte_carlo.mc_master.add_variable(mc_var)

# note: this test also covers mc_variable_random_normal.cc lines 81-83
#       in the same code section after the message

trick.stop(1)
