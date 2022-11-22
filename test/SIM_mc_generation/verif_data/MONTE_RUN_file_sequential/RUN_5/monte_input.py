monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_file_sequential/input.py').read())
monte_carlo.mc_master.monte_run_number = 5

monte_carlo.x_file_lookup[0] = 12
monte_carlo.x_file_lookup[1] = 11
monte_carlo.x_file_lookup[2] = 10
