monte_carlo.mc_master.activate("RUN_remove_variable/RUN_both_variables")
monte_carlo.mc_master.set_num_runs(1)
monte_carlo.mc_master.input_file_name = "input_a.py"

exec(open("RUN_remove_variable/variable_list.py").read())

trick.stop(1)
