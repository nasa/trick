import os

# remove write permission to the 'MONTE_RUN' directory
os.chmod("MONTE_IO_RUN_ERROR1", 0o555)

monte_carlo.mc_master.activate("IO_RUN_ERROR1")
monte_carlo.mc_master.generate_meta_data = True

print('***********************************************************************************')
print('these messages are expected:')
print('  Error I/O error')
print('  Unable to open the variable summary files for writing.')
print('  Dispersion summary will not be generated.')
print('')
print('  Warning I/O error')
print('  Unable to open file MONTE_ERROR_IO_error/MonteCarlo_Meta_data_output for writing.')
print('  Aborting generation of meta-data.')
print('***********************************************************************************')

# this simulation attempts to create good data but with the target
# directory write-protected, it can't generate the (optional) summary files.
# NOTE - we avoid the terminal failure of not being able to generate the input
#        files by having num_runs = 0, so none are attempted.
mc_var = trick.MonteCarloVariableFile( "test.x_file_lookup[0]",
                                       "Modified_data/datafile.txt",
                                       3)
monte_carlo.mc_master.add_variable(mc_var)

trick.add_read(0,"""
os.chmod('MONTE_RUN_ERROR_IO_error', 0o755)
""")
trick.stop(1)
