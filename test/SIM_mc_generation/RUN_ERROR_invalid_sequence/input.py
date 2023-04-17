monte_carlo.mc_master.activate("RUN_ERROR_invalid_sequence")
monte_carlo.mc_master.set_num_runs(1)

print('*****************************************************************************************************************************')
print('three (3) types of messages are expected:')
print('  Error Invalid sequence')
print('  Attempted to set the number of runs to 10, but the input files have')
print('  already been generated.')
print('')
print('  Error Invalid sequence')
print('  Attempted to add a new variable test.x_normal to run RUN_invalid_sequence, but the input files have already been generated.')
print('  Cannot modify input files to accommodate this new variable.')
print('  Addition of variable rejected.')
print('')
print('  Error Invalid sequence')
print('  Attempted to generate a set of input files, but this action has')
print('  already been completed.  Keeping the original set of input files.')
print('  Ignoring the later instruction.')
print('*****************************************************************************************************************************')


mc_var = trick.MonteCarloVariableRandomUniform( "test.x_uniform", 0, 10, 20)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

# Trigger the "Invalid sequence" errors in MonteCarloMaster
monte_carlo.mc_master.prepare_input_files()

# Change run-number after prepping inputs
monte_carlo.mc_master.set_num_runs(10)

# Add a new variable after prepping inputs
mc_var = trick.MonteCarloVariableRandomNormal( "test.x_normal", 2, 10, 2)
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

trick.stop(1)
