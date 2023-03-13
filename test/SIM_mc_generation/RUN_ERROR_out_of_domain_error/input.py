monte_carlo.mc_master.activate("RUN_ERROR_out_of_domain_error")
monte_carlo.mc_master.set_num_runs(1)

print('***********************************************************************')
print('these messages are expected:')
print('  Error Out-of-domain error')
print('  Negative double-sided truncation specified for variable test.x_normal')
print('  truncate() must receive either two limits or one positive limit!')
print('  Using absolute value of limit.')
print('***********************************************************************')


mc_var = trick.MonteCarloVariableRandomNormal( "test.x_normal" )
# the next command is the source of the error!
mc_var.truncate(-1.0)
monte_carlo.mc_master.add_variable(mc_var)

trick.stop(1)
