monte_carlo.mc_master.activate("RUN_file_skip2")
# For regression monte_carloing, use 5 runs
# For verification, setting this value to 250 results in 2 duplications.
monte_carlo.mc_master.set_num_runs(5)
#monte_carlo.mc_master.set_num_runs(250)


mc_var = trick.MonteCarloVariableFile( "test.x_file_lookup[0]",
                                       "Modified_data/single_col_1.txt",
                                       0,
                                       0)
mc_var.max_skip = 1
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloVariableFile( "test.x_file_lookup[1]",
                                       "Modified_data/single_col_2.txt",
                                       0,
                                       0)
mc_var.max_skip = 2
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloVariableFile( "test.x_file_lookup[2]",
                                       "Modified_data/single_col_3.txt",
                                       0,
                                       0)
mc_var.max_skip = 3
mc_var.thisown = False
monte_carlo.mc_master.add_variable(mc_var)

trick.stop(1)
