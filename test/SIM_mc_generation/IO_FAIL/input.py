import os, shutil

# remove write permission to the 'MONTE_RUN' directory
os.chmod("MONTE_IO_FAIL", 0o555)

monte_carlo.mc_master.activate("IO_FAIL")
monte_carlo.mc_master.generate_meta_data = True
monte_carlo.mc_master.set_num_runs(1)

print('*********************************************************************************')
print('these messages are expected:')
print('  Error I/O error')
print('  Unable to open the variable summary files for writing.')
print('  Dispersion summary will not be generated.')
print('')
print('  DIAGNOSTIC: Fatal Error I/O error')
print('  Unable to open file MONTE_FAIL_IO_error2/RUN_0/monte_input.py for writing.')
print('*********************************************************************************')

# this simulation attempts to create good data but with the target
# directory write-protected, it can't generate the required input files.
mc_var = trick.MonteCarloVariableFile( "test.x_file_lookup[0]",
                                       "Modified_data/datafile.txt",
                                       3)
monte_carlo.mc_master.add_variable(mc_var)

trick.stop(1)
