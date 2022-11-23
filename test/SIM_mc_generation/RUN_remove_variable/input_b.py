monte_carlo.mc_master.activate("RUN_remove_variable/RUN_one_variable")
monte_carlo.mc_master.set_num_runs(1)
monte_carlo.mc_master.input_file_name = "input_b.py"

exec(open("RUN_remove_variable/variable_list.py").read())

# execute remove_variable() (success path)
# code coverage for mc_master.cc, remove_variable(), lines 288-289
monte_carlo.mc_master.remove_variable("test.x_fixed_value_int")

trick.stop(1)
