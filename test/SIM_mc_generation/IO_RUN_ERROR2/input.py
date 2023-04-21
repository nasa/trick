import os

# remove write permission to the 'RUN_0' directory
os.chmod("MONTE_IO_RUN_ERROR2/RUN_0", 0o500)

monte_carlo.mc_master.activate("IO_RUN_ERROR2")
monte_carlo.mc_master.generate_meta_data = True
monte_carlo.mc_master.set_num_runs(1)

print('***********************************************************************************')
print('this message is expected:\n'+
      '  Error Output failure\n'+
      '  Failed to record summary data for run 0.')
print('***********************************************************************************')

# this simulation attempts to create good data but with the target
# directory write-protected, it can't generate the (optional) summary files.
# NOTE - we avoid the terminal failure of not being able to generate the input
#        files by having num_runs = 0, so none are attempted.
mc_var = trick.MonteCarloVariableFile( "test.x_file_lookup[0]",
                                       "Modified_data/datafile.txt",
                                       3)
monte_carlo.mc_master.add_variable(mc_var)

trick.stop(1)
