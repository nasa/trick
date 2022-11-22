monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('FAIL_malformed_data_file/input.py').read())
monte_carlo.mc_master.monte_run_number = 0
