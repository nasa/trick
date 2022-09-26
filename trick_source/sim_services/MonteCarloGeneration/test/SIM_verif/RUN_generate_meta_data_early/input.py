test.mc_master.activate("RUN_generate_meta_data_early")
test.mc_master.set_num_runs(1)
test.mc_master.generate_meta_data = True
test.mc_master.input_file_name = "input.py"

exec(open("Modified_data/monte_variables.py").read())

test.mc_master.collate_meta_data()
