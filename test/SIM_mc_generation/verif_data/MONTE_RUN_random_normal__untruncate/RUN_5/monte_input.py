monte_carlo.mc_master.active = True
monte_carlo.mc_master.generate_dispersions = False

exec(open('RUN_random_normal__untruncate/input.py').read())
monte_carlo.mc_master.monte_run_number = 5

monte_carlo.x_normal_trunc[0] = 10.72611121241102
monte_carlo.x_normal_trunc[1] = 10.72611121241102
