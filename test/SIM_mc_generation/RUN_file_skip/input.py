monte_carlo.mc_master.activate("RUN_file_skip")
monte_carlo.mc_master.set_num_runs(10)

mc_var = trick.MonteCarloVariableFile( "test.x_file_lookup[0]",
                                        "Modified_data/datafile.txt",
                                        3)
mc_var.thisown = False
mc_var.max_skip = 3
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloVariableFile( "test.x_file_lookup[1]",
                                        "Modified_data/datafile.txt",
                                        2)
mc_var.thisown = False
mc_var.max_skip = 3
monte_carlo.mc_master.add_variable(mc_var)

mc_var = trick.MonteCarloVariableFile( "test.x_file_lookup[2]",
                                        "Modified_data/datafile.txt",
                                        1)
mc_var.thisown = False
mc_var.max_skip = 3
monte_carlo.mc_master.add_variable(mc_var)


trick.stop(1)
