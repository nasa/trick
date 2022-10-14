test.mc_master.activate("RUN_ERROR_invalid_name")
test.mc_master.set_num_runs(1)

print('*********************************************************************')
print('this message is expected:\n'+
      '  Error Invalid name\n' +
      '  Could not find MonteCarlo variable with name test.x_uniform.\n'+
      '  Returning a NULL pointer.')
print('*********************************************************************')

# empty monte carlo master without any variables.
# lets ask it find us a variable.
test.mc_master.find_variable("test.x_uniform")

trick.stop(1)
