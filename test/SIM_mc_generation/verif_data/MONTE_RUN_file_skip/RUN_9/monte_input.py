monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_file_skip/input.py').read())
monte_carlo.mc_master.monte_run_number = 9

monte_carlo.x_file_lookup[0] = 32
monte_carlo.x_file_lookup[1] = 31
monte_carlo.x_file_lookup[2] = 30
