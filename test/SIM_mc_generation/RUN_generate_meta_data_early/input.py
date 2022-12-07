monte_carlo.mc_master.activate("RUN_generate_meta_data_early")
monte_carlo.mc_master.set_num_runs(1)
monte_carlo.mc_master.generate_meta_data = True
monte_carlo.mc_master.input_file_name = "input.py"

exec(open("Modified_data/monte_variables.py").read())

monte_carlo.mc_master.collate_meta_data()
